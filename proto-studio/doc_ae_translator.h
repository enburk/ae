#pragma once
#include "doc_ae_synthesis_in.h"
#include "doc_ae_synthesis_out.h"
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::translator
{
    using synthesis::entity;
    using synthesis::statement;

    array<str> proceed (syntax::analysis::data& data)
    {
        statement module = 
        synthesis::in(data.module);

        entity body;
        entity main;
        main.head = "void main_()";

        for (auto& statement: module.body)
            synthesis::out(statement,
                statement.kind == "expression" ?
                main.body:
                body.body);

        if (main.body.empty())
            main.body +=
            entity{};

        array<str> lines;
        for (auto&& e: body.body) e.print(lines);
        for (auto&& e: main.body) e.print(lines);
        return lines;
    }
}

