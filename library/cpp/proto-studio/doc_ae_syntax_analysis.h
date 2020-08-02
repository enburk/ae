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
        array<token> tokens;
        array<statement> statements;
        array<path> dependencies;
        time compile_time;
        scope scope;
        report log;
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
        record.compile_time = now;
        record.log.info(nullptr, source.string());
        record.tokens = lexica::parse(model);
        pass1 (record, source, record.tokens);
        return true;
    };

    void pass1 (data & record, path source, array<token> & tokens)
    {
        auto & log = record.log;

        record.statements = 
            statementor(log).proceed(
            parser(log).proceed(
            tokens));

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
                    std::string s = that.param->text;
                    s.pop_back(); s.erase(0,1); s += ".ae";
                    auto path = source.parent_path() / s;
                    if (record.dependencies.contains(path)) continue;

                    record.dependencies.try_emplace(path);
                    auto & data = sources[path]; pass1(data, path);
                    for (auto p : data.dependencies) if (p != source)
                        record.dependencies.try_emplace(p);
                }
            }
        }

        record.scope.named["global"].fill(record.statements, log);
    }

    void pass2 (data & record)
    {
        auto & log = record.log;
        if (not log.errors.empty()) return;

        //for (auto path : record.dependencies)
        //    record.scope.add(repo[path].scope);

    }

    std::mutex mutex; // makes the thread safety of doc::repo pointless

    // run/compilation button
    const data & proceed (path source)
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

    // live editor analysis
    const data & proceed (path source, array<token> & tokens)
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
}

