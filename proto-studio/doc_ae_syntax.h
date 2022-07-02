#pragma once
#include "doc_text_text.h"
namespace doc::ae::syntax
{
    using text::token;
    using text::report;

    struct statement;
    struct expression;
    struct type
    {
        str name;
        str kind;
    };

    struct brackets
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<expression> list;
    };

    struct nameunit
    {
        token* coloncolon = nullptr;
        token* identifier = nullptr;
        array<brackets> argss;
    };

    struct terminal { token* token = nullptr; type type; };

    struct namepack { array<nameunit> names; type type; };

    struct operands { array<expression> list; };

    struct expression
    {
        std::variant
        <
            terminal,
            namepack,
            brackets,
            operands
        >
        variant;
        type
        type;
    };

    struct parameter
    {
        token* name = nullptr;
        namepack typexpr;
        expression value;
        type type;
    };

    struct parameters
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<parameter> list;
    };

    struct statement
    {
        str kind;
        array<token*> names;
        parameters args;
        expression expr;
        namepack typexpr;
        array<statement> body;
        type type;

        statement* outer = nullptr;
        std::map<str, statement*> modules;
        std::multimap<str, statement*> members;

        //std::map<namepack, namepack> dependencies;

        str schema;
        str source;
    };
}

