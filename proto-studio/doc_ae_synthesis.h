#pragma once
#include "doc_text_text.h"
#include "doc_ae_syntax.h"
namespace doc::ae::synthesis
{
    using syntax::type;

    struct expression;

    struct brackets
    {
        str opening;
        str closing;
        array<expression> list;
    };

    struct terminal { str text; type type; };

    struct nameunit { str identifier; array<brackets> argss; };

    struct namepack { array<nameunit> names; type type; };

    struct operands { array<expression> list; };

    struct namelist { array<str> list; };

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
        str name;
        namepack typexpr;
        expression value;
        type
        type;
    };
    struct parameters { array<parameter> list; };

    struct statement
    {
        str kind;
        namelist names;
        parameters args;
        expression expr;
        namepack typexpr;
        array<statement> body;
        type type;
        str schema;
        str source;
    };
}

