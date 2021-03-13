#pragma once
#include "data_struct_array.h"
#include "data_struct_string.h"
#include "pix_text.h"
namespace doc
{
    using namespace data;

    struct place
    {
        int line = 0;
        int offset = 0;
        auto operator <=> (const place & p) const = default;
    };

    struct range
    {
        place from, upto;
        bool empty () const { return from == upto; }
        bool operator == (const range & r) const = default;
        bool operator != (const range & r) const = default;
    };
}

namespace doc::view
{
    using pix::text::style;
    using pix::text::style_index;
    using pix::text::format;

    struct token
    {
        str text; style_index style; str info;
        bool operator != (const token & d) const = default;
        bool operator == (const token & d) const = default;
    };

    struct line
    {
        format format; array<token> tokens;
        bool operator != (const line & d) const = default;
        bool operator == (const line & d) const = default;
    };

    struct model : polymorphic
    {
        array<line> lines;
        virtual void set (style s, format f) = 0;
    };
}


  