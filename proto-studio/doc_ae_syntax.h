#pragma once
#include "doc.h"
#include "doc_text_text.h"
namespace doc::ae::syntax
{
    using text::token;
    using text::report;

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

    struct expression
    {
        std::variant
        <
//          terminal,
            namepack,
            brackets
        >
        variant;
    };

    struct parameter
    {
        token* name;
        namepack type;
        expression value;
    };

    struct definition
    {
        token* name = nullptr;
        token* kind = nullptr;
        array<statement> body;
        array<parameter> args;
        namepack type;
    };

    struct statement
    {
        std::variant
        <
            expression,
            definition
        >
        variant;
        str schema;
        str source;
    };
}

