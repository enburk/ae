#pragma once
#include <map>
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

    struct entity
    {
        str name, kind, info;
        std::map<str,str> attr;
        array<token> head;
        array<entity> body;
        array<token> tail;

        explicit entity() = default;
        explicit entity(str s) { head += token{s}; }
        explicit entity(array<token> tt) : head(tt) {}
        explicit entity(str name, str kind) : name(name), kind(kind) {}
    };

    struct report
    {
        struct message { token* token = nullptr; str kind, what; };
        array <message> messages;
        array <message> errors;

        void error (token* token, str what) {
            messages += message{token, "error", what};
            errors += message{token, "error", what};
            if (token) token->kind = "error";
        }

        void info (token* token, str what) {
            messages += message{token, "info", what};
        }

        str operator () () const
        {
            str s; for (auto [token, kind, what] : messages)
            {
                if (kind == "error") s += "<font color=#B00020>";

                if (token)
                    s += "("
                    + std::to_string(token->range.from.line+1) + ":"
                    + std::to_string(token->range.from.offset+1) + ") ";

                s += what;

                if (kind == "error") s += "</font>";

                s += "<br>";
            }
            return s;
        }
    };
}


  