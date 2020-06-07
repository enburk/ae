#pragma once
#include "doc.h"
namespace doc::ae::syntax
{
    struct scope;

    struct element;

    struct brackets
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<element> body;
    };

    struct unqualified_name
    {
        token* coloncolon = nullptr;
        token* identifier = nullptr;
        array<brackets> params;
    };

    struct name { array<unqualified_name> names; };

    struct statement
    {
        str kind, id; name type;
        scope * scope = nullptr;
        array<element> elements;
        array<statement> body;
    };

    using variant = std::variant
    <
        token*,
        brackets,
        name
    >;

    struct element : variant {
        using variant::variant;
        element(variant v) : variant(std::move(v)) {}
        auto operator = (variant v) { variant::
            operator = (std::move(v));
            return *this;
        }
    };
}

