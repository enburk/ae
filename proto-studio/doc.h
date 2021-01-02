#pragma once
#include "data_struct_array.h"
#include "data_struct_string.h"
namespace doc
{
    using pix::text::style;
    using pix::text::style_index;
    using pix::text::format;

    struct token
    {
        str text; style_index style;

        bool operator != (const token & d) const = default;
        bool operator == (const token & d) const = default;
    };

    struct line
    {
        format format; array<token> tokens;

        bool operator != (const line & d) const = default;
        bool operator == (const line & d) const = default;
    };
}


  