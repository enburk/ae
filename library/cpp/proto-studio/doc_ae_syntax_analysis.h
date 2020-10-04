#pragma once
#include <chrono>
#include <fstream>
#include <filesystem>
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
    void pass1 (data & record, path source);

    void pass2 (data & record, array<statement> & body);
    void pass2 (data & record)
    {
        if (record.cpp  ) return;
        if (record.pass2) return; else
            record.pass2 = true;

        for (auto p : record.dependencies)
            pass2(sources[p]);

        if (record.source != path{})
            record.log.info(nullptr, record.source.string() + " pass 2");

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

    void pass2 (data & record, scope* scope, loop_for & s);
    void pass2 (data & record, scope* scope, loop_while & s);
    void pass2 (data & record, scope* scope, expression & e);
    void pass2 (data & record, scope* scope, conditional & s);
    void pass2 (data & record, scope* scope, declaration & s);
    void pass2 (data & record, scope* scope, definition & s);
    void pass2 (data & record, scope* scope, subroutine & s);
    void pass2 (data & record, scope* scope, directive & s);
    void pass2 (data & record, scope* scope, pragma & s);
    void pass2 (data & record, array<statement> & body)
    {
        for (auto & st : body)
        {
            std::visit(aux::overloaded
            {
                [&](noop        &s) {},
                [&](loop_for    &s) { pass2(record, st.scope, s); },
                [&](loop_while  &s) { pass2(record, st.scope, s); },
                [&](expression  &s) { pass2(record, st.scope, s); },
                [&](conditional &s) { pass2(record, st.scope, s); },
                [&](declaration &s) { pass2(record, st.scope, s); },
                [&](definition  &s) { pass2(record, st.scope, s); },
                [&](subroutine  &s) { pass2(record, st.scope, s); },
                [&](directive   &s) { pass2(record, st.scope, s); },
                [&](pragma      &s) { pass2(record, st.scope, s); },
            },
            st.variant);
        }
    }
}

