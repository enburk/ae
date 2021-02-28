#pragma once
#include <chrono>
#include <fstream>
#include <filesystem>
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
#include "doc_ae_syntax_dependencies.h"
namespace doc::ae::syntax::analysis
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    report events;

    struct data
    {
        scope scope;
        array<statement> statements;
        array<token*> dependencies;
        array<path> dependees;
        report log, log2, log3;
        bool passed2 = false;
        bool passed3 = false;
        time time;
        path path;

        void pass1 (array<token> & tokens)
        {
            events.info("pass1 "
                + path.string());

            log.clear();
            statements =
                schema(
                parser(tokens
                , log).output
                , log).output
                ;
            scope = decltype(scope)(
                statements,
                log);
            dependencies =
            dependencies::collect(statements);
            passed2 = false;
            passed3 = false;
        }

        void pass2 ()
        {
            if (passed2) return;
            else passed2 = true;
            events.info("pass2 "
                + path.string());

            log2.clear();
            dependees =
            dependencies::resolve(
                path.parent_path(),
                dependencies, log2);
        }

        void pass3 ()
        {
            if (passed3) return;
            else passed3 = true;
            events.info("pass3 " +
                path.string());

            log3.clear();
            time = time::clock::now();
        }
    };

    std::map<path, data*> datae;

    void pass1 (path path, data & data, array<token> & tokens)
    {
        datae[path] = &data;
        data.path = path;
        data.pass1(tokens);
    }

    void pass2 (path path)
    {
        datae[path]->pass2();
    }

    void pass3 (path path)
    {
        // interscopes
        datae[path]->pass3();
    }

    void remove (path path)
    {
        datae.erase(path);
    }
}

