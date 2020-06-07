#pragma once
#include <cassert>
#include <compare>
#include <variant>
#include "../aux_string.h"
#include "../aux_utils.h"
namespace doc
{
    using namespace aux;

    using glyph = str;

    struct text
    {
        array<array<glyph>> lines;

        bool operator == (const text & t) const { return lines == t.lines; }
        bool operator != (const text & t) const { return lines != t.lines; }

        explicit text (str s = "") {
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

    struct entity
    {
        str name, kind, info;
        std::map<str,str> attr;
        array<token> head;
        array<entity> body;
        array<token> tail;
    };

    inline
    array<std::pair<token*, str>> errors;
    void error (token* token, str what) {
        errors += std::pair{token, what};
        if (token) token->kind = "error";
    }
}


  