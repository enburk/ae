#pragma once
#include "doc_text_glyph.h"
namespace doc::text
{
    struct text
    {
        array<array<glyph>> lines;

        bool operator == (const text & t) const = default;
        bool operator != (const text & t) const = default;

        text () = default;
        explicit text (str s) {
            lines.reserve(s.size()/80);
            for (str line : s.split_by("\n"))
                lines += glyph::parse(line);
        }
        str string () const {
            str s; for (const auto & line : lines)
                s += doc::text::string(line) + "\n";
            if (s != "") s.pop_back();
            return s;
        }
    };

    struct token
    {
        str text, kind; range range;
        void operator += (const glyph & g) { text += g.string(); range.upto.offset++; }
        bool operator != (const token & t) const = default;
        bool operator == (const token & t) const = default;
    };
}


  