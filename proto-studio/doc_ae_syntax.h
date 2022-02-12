#pragma once
#include "doc_text_text.h"
namespace doc::ae::syntax
{
    using text::token;
    using text::report;

    struct scope;
    struct statement;
    struct expression;

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
        array<brackets> args;
    };

    struct namepack { array<nameunit> names; };

    struct operands { array<expression> list; };

    struct terminal { token* token = nullptr; };

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
        scope* scope;
        str typeline;
    };

    struct parameter
    {
        token* name = nullptr;
        namepack type;
        expression value;
        str typeline;
    };

    struct parameters
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<parameter> list;
    };

    struct statement
    {
        token* name = nullptr;
        array<statement> body;
        parameters args;
        expression expr;
        namepack type;
        std::map<namepack, namepack> dependencies;
        scope* scope;
        str schema;
        str source;
        str kind;
    };
}

