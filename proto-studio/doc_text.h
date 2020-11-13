#pragma once
#include "aux_string.h"
#include "aux_utils.h"
namespace doc::text
{
    using namespace aux;

    using glyph = str;

    struct text
    {
        array<array<glyph>> lines;

        bool operator == (const text & t) const { return lines == t.lines; }
        bool operator != (const text & t) const { return lines != t.lines; }

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
        bool operator == (range r) const { return from == r.from && upto == r.upto; }
        bool operator != (range r) const { return from != r.from || upto != r.upto; }
    };

    struct token
    {
        str text, kind; range range;
        void operator += (const glyph & g) { text += g; range.upto.offset++; }
        bool operator != (const token & t) const { return !(*this == t); }
        bool operator == (const token & t) const { return
            text  == t.text &&
            kind  == t.kind &&
            range == t.range;
        }
    };
}


  