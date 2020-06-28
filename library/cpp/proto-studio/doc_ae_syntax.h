#pragma once
#include "doc.h"
namespace doc::ae::syntax
{
    struct element
    {
        str kind, name;
        token* opening = nullptr;
        token* closing = nullptr;
        array<element> elements;
    };

    struct statement;
    struct expression;
    struct conditional;
    struct loop;

    struct number  { token* token; };
    struct literal { token* token; };

    struct brackets
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<statement> list;
    };

    struct named_unit
    {
        token* coloncolon = nullptr;
        token* identifier = nullptr;
        array<brackets> parameters;
    };

    struct named_pack { array<named_unit> units; };

    struct operation
    {
        token* token = nullptr;
        array<expression> operands;
        str kind;
    };

    struct expression_if
    {
        token* title = nullptr;
        array<expression> condition; // array prevents infinite recursion
        array<expression> then_body; // array prevents infinite recursion
        array<expression> else_body; // array prevents infinite recursion
    };

    struct expression_for
    {
        token* title = nullptr;
        token* index = nullptr;
        array<expression> container; // array prevents infinite recursion
    };

    struct expression
    {
        std::variant
        <
            number,
            literal,
            named_pack,
            operation,
            brackets,
            expression_if,
            expression_for
        >
        variant;
    };

    struct conditional
    {
        token* title = nullptr;
        expression condition;
        array<statement> then_body;
        array<statement> else_body;
    };

    struct loop
    {
        token* title = nullptr;
        expression condition;
        array<statement> initial;
        array<statement> iterational;
        array<statement> body;
    };

    struct pragma
    {
        token* title = nullptr;
        array<expression> arguments;
    };

    struct declaration
    {
        token* name = nullptr;
        named_pack type;
        array<statement> body;
    };

    struct statement
    {
        std::variant
        <
            loop,
            expression,
            conditional,
            declaration,
            pragma
        >
        variant;
    };
}

