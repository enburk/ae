#pragma once
#include "data_struct_array.h"
#include "data_struct_string.h"
namespace doc::text
{
    using namespace data;

    using glyph = str;

    struct text
    {
        array<array<glyph>> lines;

        bool operator == (const text & t) const = default;
        bool operator != (const text & t) const = default;

        text () = default;
        explicit text (str s) {
            lines.reserve(s.size()/80);
            for (str line : s.split_by("\n"))
                lines += unicode::glyphs(line);
        }
        str string () const {
            str s; for (const auto & line : lines)
                s += str(line, "") + "\n";
            if (s != "") s.pop_back();
            return s;
        }
    };

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

    struct token
    {
        str text, kind; range range;
        void operator += (const glyph & g) { text += g; range.upto.offset++; }
        bool operator != (const token & t) const = default;
        bool operator == (const token & t) const = default;
    };
}


  