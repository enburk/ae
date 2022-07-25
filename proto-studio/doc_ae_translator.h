#pragma once
#include "doc_ae_synthesis_in.h"
#include "doc_ae_synthesis_out.h"
#include "doc_ae_synthesis_proc.h"
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::translator
{
    using synthesis::entity;
    using synthesis::statement;

    array<str> proceed (syntax::analysis::data& data, bool exe)
    {
        statement module = 
        synthesis::in(data.module);

        std::atomic<bool> cancel;
        synthesis::proceed(module, cancel);

        entity body;
        entity main;
        str name = exe ? "ae" : data.name.text;
        body.head = "namespace " + name;
        main.head = "void main_()";

        for (auto& statement: module.body)
            synthesis::out(statement,
                statement.kind == "type" or
                statement.kind == "singleton" or
                statement.kind == "function" or
                statement.kind == "mutation" or
                statement.kind.contains("operator")?
                body.body:
                main.body);

        if (main.body.empty())
            main.body +=
            entity{};

        body.body += main;

        array<str> lines;
        body.print(lines);
        return lines;
    }
}

