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

    struct scope;
    struct statement;
    struct expression;
    struct conditional;
    struct loop;

    struct number  { token* token; };
    struct symbol  { token* token; };
    struct literal { token* token; };

    struct brackets
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<expression> list;
    };

    struct named_unit
    {
        token* coloncolon = nullptr;
        token* identifier = nullptr;
        array<brackets> arguments;
    };

    struct named_pack { array<named_unit> units; };

    struct operation
    {
        token* title = nullptr;
        array<expression> operands;
        str kind;
    };

    struct expression_if
    {
        token* title = nullptr;
        array<expression> condition;
        array<expression> then_body;
        array<expression> else_body;
    /// array prevents infinite recursion
    };

    struct expression_for
    {
        token* title = nullptr;
        array<token*> names;
        array<expression> range;
    /// array prevents infinite recursion
    };

    struct expression
    {
        std::variant
        <
            number,
            symbol,
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

    struct loop_for
    {
        token* title = nullptr;
        array<token*> names;
        named_pack range;
        array<statement> body;
    };

    struct loop_while
    {
        token* title = nullptr;
        expression condition;
        array<statement> body;
    };

    struct pragma
    {
        token* title = nullptr;
        token* param = nullptr;
    };

    struct parameter
    {
        token* name;
        named_pack type;
        expression value;
    };

    struct subroutine
    {
        str kind;
        token* title = nullptr;
        token* name = nullptr;
        named_pack type;
        array<parameter> parameters;
        array<statement> body;
    };

    struct declaration
    {
        str kind;
        named_pack type;
        array<token*> names;
        array<statement> body;
    };

    struct statement
    {
        std::variant
        <
            loop_for,
            loop_while,
            expression,
            conditional,
            declaration,
            subroutine,
            pragma
        >
        variant;
        str schema;
        scope* scope = nullptr;
    };
}

