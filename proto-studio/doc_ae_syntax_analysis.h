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

    struct data
    {
        scope scope;
        array<statement> statements;
        array<str> dependencies;
        array<path> dependees;
        report log;
        bool passed2 = false;
        bool passed3 = false;
        time time;
        path path;

        void pass1 (array<token> & tokens)
        {
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
            dependees =
            dependencies::resolve(path, dependencies, log);
        }

        void pass3 ()
        {
            if (passed3) return;
            else passed3 = true;
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

