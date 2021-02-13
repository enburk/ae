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
    };

    struct parameter
    {
        token* name;
        namepack type;
        expression value;
    };

    struct parameters
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<parameter> list;
    };

    struct definition
    {
        token* name = nullptr;
        array<statement> body;
        parameters args;
        namepack type;
        str kind;
        str schema;
        str source;
    };

    struct statement
    {
        std::variant
        <
            expression,
            definition
        >
        variant;
    };
}

