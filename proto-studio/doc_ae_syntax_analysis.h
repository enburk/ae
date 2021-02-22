#pragma once
#include <chrono>
#include <fstream>
#include <filesystem>
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
namespace doc::ae::syntax::analysis
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    struct data
    {
        scope scope;
        array<statement> statements;
        array<str> dependencies;
        array<path> dependers;
        array<path> dependees;
        report log;
    };

    std::map<path, data*> datae;

    void pass1 (path path, data & data, array<token> & tokens)
    {
        datae[path] = &data;

        data.log.clear();
        data.statements =
            schema(
            parser(tokens
            , data.log).output
            , data.log).output
            ;
        data.scope = scope(
            data.statements,
            data.log);
    }

    void pass2 (path path, data & data)
    {
    }

    void remove (path path)
    {
        auto dependers =
        datae[path]->dependers;
        datae.erase(path);

        for (auto depender : dependers) {
            datae[depender]->dependees.try_erase(path);
            pass2(depender, *datae[depender]);
        }
    }
}

