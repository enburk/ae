#pragma once
#include <chrono>
#include <fstream>
#include <filesystem>
#include "doc_text_repo.h"
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
namespace doc::ae::syntax::analysis
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    struct data
    {
        path source;
        array<token> tokens;
        array<statement> statements;
        array<path> dependencies;
        time compile_time;
        scope scope;
        report log;
        bool cpp = false;
        bool pass2 = false;
        bool synthesized = false;
    };

    inline std::map<path, data> sources;

    inline array<path> standard_library;

    void pass1 (data & record, path source, array<token> &);
    bool pass1 (data & record, path source)
    {
        time edit_time;
        text_model model;
        time now = time::clock::now();
        doc::repo::edittime(source, edit_time).value();

        if (record.compile_time >= edit_time)
        {
            bool up_to_date = true;

            for (auto path : record.dependencies)
            {
                doc::repo::edittime(path, edit_time).value();
                if (sources[path].compile_time < edit_time) {
                    up_to_date = false;
                    break;
                }
            }

            if (up_to_date) return false;
        }

        doc::repo::load(source, model).value();

        record = data{};
        record.source = source;
        record.compile_time = now;
        record.log.info(nullptr, source.string());
        record.tokens = lexica::parse(model);

        if (source.string().ends_with(".c++")) {
            report log = cpp::syntax::analysis::proceed(source, record.tokens).log;
            record.log.messages += log.messages;
            record.log.errors += log.errors;
            record.cpp = true;
            return true;
        }

        pass1 (record, source, record.tokens);
        return true;
    };

    void pass1 (data & record, path source, array<token> & tokens)
    {
        auto & log = record.log;

        record.statements = statementor(parser(tokens
            , log).output
            , log).output
            ;

        if (not log.errors.empty()) return;

        if (not standard_library.contains(source))
        {
            for (auto path : standard_library)
            {
                record.dependencies.try_emplace(path);
                auto & data = sources[path]; pass1(data, path);
                for (auto p : data.dependencies) if (p != source)
                    record.dependencies.try_emplace(p);
            }
        }

        for (auto & s : record.statements)
        {
            if (std::holds_alternative<pragma>(s.variant)) {
                auto & that = std::get<pragma>(s.variant);
                if (that.title->text == "using")
                {
                    std::string fn;
                    if (aux::got<literal>(that.arg.variant)) { fn =
                        std::get<literal>(that.arg.variant).token->text;
                        fn.pop_back(); fn.erase(0,1); // quots
                    }
                    else
                    if (aux::got<named_pack>(that.arg.variant)) { auto units =
                        std::get<named_pack>(that.arg.variant).units;
                        if (units.size() != 1) continue;
                        if (units[0].arguments.size() != 0) continue;
                        fn = units[0].identifier->text;
                    }
                    if (not fn.ends_with(".c++")) fn += ".ae";
                    auto path = source.parent_path() / fn;
                    if (record.dependencies.contains(path)) continue;

                    record.dependencies.try_emplace(path);
                    auto & data = sources[path]; pass1(data, path);
                    for (auto p : data.dependencies) if (p != source)
                        record.dependencies.try_emplace(p);
                }
            }
        }

        record.scope.fill(record.statements);
    }

    void pass2 (data & record, array<statement> & body);
    void pass2 (data & record)
    {
        if (record.pass2 or record.cpp) return;
            record.pass2 = true;

        for (auto p : record.dependencies)
            pass2(sources[p]);

        // record.log.info(nullptr, record.source.string() + " pass 2");

        if (not record.log.errors.empty()) return; record.scope.check();
        if (not record.log.errors.empty()) return;

        pass2(record, record.statements);
    }

    std::mutex mutex; // makes thread safety of doc::repo pointless

    const data & proceed (path source) // run/compilation button
    {
       std::unique_lock guard{mutex};

        data & record = sources[source];
        try
        {
            record.log = report{};
            pass1(record, source);
            pass2(record);
            for (auto p : record.dependencies) {
                auto & dependency = sources[p];
                if (not dependency.log.errors.empty() or
                    dependency.compile_time > record.compile_time)
                    record.log.messages += dependency.log.messages;
                    record.log.errors += dependency.log.errors;
            }
        }
        catch (const aux::exception & e) {
            record.log.error(nullptr, e.what());
        }
        return record;
    }

    const data & proceed (path source, array<token> & tokens) // live editor analysis
    {
       std::unique_lock guard{mutex};

        data & record = sources[source];
        try
        {
            record = data{};
            record.tokens = tokens;
            record.compile_time = time::clock::now();
            pass1(record, source, tokens);
            pass2(record);
        }
        catch (const aux::exception & e) {
            record.log.error(nullptr, e.what());
        }
        return record;
    }

    void pass2 (data & record, loop_for & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, loop_while & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, expression & e)
    {
        std::visit(aux::overloaded
        {
            [&](number  &v) {},
            [&](symbol  &v) {},
            [&](literal &v) {},
            [&](named_pack &v)
            {
            },
            [&](operation &v)
            {
                for (auto & o : v.operands)
                    pass2(record, o);

                //deque<expression> deque(v.operands);
                //
                //v.operands.clear();
                //v.operands += deque.front();
                //deque.pop_front();

                if (v.operands.size() == 3 and
                    aux::got<symbol>(v.operands[1].variant)) { v.title =
                    std::get<symbol>(v.operands[1].variant).token;
                    v.operands.from(1).upto(2).erase();
                }
            },
            [&](brackets &v)
            {
            },
            [&](expression_if &v)
            {
                for (auto & e : v.condition) pass2(record, e);
                for (auto & e : v.then_body) pass2(record, e);
                for (auto & e : v.else_body) pass2(record, e);
            },
            [&](expression_for &v)
            {
                for (auto & e : v.range) pass2(record, e);
            },
            [&](lambda &v)
            {
                pass2(record, v.body);
            },
        },
        e.variant);
    }
    void pass2 (data & record, conditional & s)
    {
        pass2(record, s.then_body);
        pass2(record, s.else_body);
    }
    void pass2 (data & record, declaration & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, definition & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, subroutine & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, pragma & s)
    {
    }
    void pass2 (data & record, array<statement> & body)
    {
        for (auto & s : body)
        {
            std::visit(aux::overloaded
            {
                [&](noop        &s) {},
                [&](loop_for    &s) { pass2(record, s); },
                [&](loop_while  &s) { pass2(record, s); },
                [&](expression  &s) { pass2(record, s); },
                [&](conditional &s) { pass2(record, s); },
                [&](declaration &s) { pass2(record, s); },
                [&](definition  &s) { pass2(record, s); },
                [&](subroutine  &s) { pass2(record, s); },
                [&](pragma      &s) { pass2(record, s); },
            },
            s.variant);
        }
    }
}

