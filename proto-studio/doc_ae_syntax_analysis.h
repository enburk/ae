#pragma once
#include <chrono>
#include <fstream>
#include <filesystem>
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
#include "doc_ae_syntax_visitor.h"
namespace doc::ae::syntax::analysis
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    struct data
    {
        scope scope;
        array<statement> statements;
        array<path> dependers;
        array<path> dependees;
        report log;

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

            array<str> deps;
            visitor visitor;
            visitor.on_name = [&deps](namepack& n)
            {
                if (n.names.size() > 0 and
                    n.names[0].coloncolon) deps.try_emplace(
                    n.names[0].identifier->text);
            };
            visitor.pass(statements);
        }
    };

    std::map<path, data*> datae;

    void pass1 (path fname, data & data, array<token> & tokens)
    {
        datae[fname] = &data; data.pass1(tokens);
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

