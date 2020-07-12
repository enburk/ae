﻿#pragma once
#include "doc.h"
namespace doc::ae::lexica
{
    inline constexpr bool ascii (char c) { return c >= ' ' && c <= '~'; }
    inline constexpr bool space (char c) { return c == ' ' || c =='\t'; }
    inline constexpr bool digit (char c) { return c >= '0' && c <= '9'; }
    inline constexpr bool alpha (char c) { return c >= 'A' && c <= 'Z' ||
                                                  c >= 'a' && c <= 'z'; }

    inline bool ascii (str c) { return c.size() == 1 && ascii(c[0]); }
    inline bool space (str c) { return c.size() == 1 && space(c[0]); }
    inline bool digit (str c) { return c.size() == 1 && digit(c[0]); }
    inline bool alpha (str c) { return c.size() == 1 && alpha(c[0]); }
    inline bool brace (str c) { return
        c == "{" or c == "(" or c == "[" or
        c == "}" or c == ")" or c == "]";
    }

    inline array<token> parse (const text & text)
    {
        array<token> tokens; token t;

        for (auto [line, n] : text.lines.whole())
        {
            for (auto [c, offset] : line.whole())
            {
                bool same = false;

                if (t.kind == "comment") same = true; else
                if (t.kind == "space"  ) same = space(c); else
                if (t.kind == "name"   ) same = alpha(c) || digit(c) || c == "."; else
                if (t.kind == "number" ) same = alpha(c) || digit(c) || c == "."; else
                if (t.kind == "symbol" ) same =
                    not alpha(c) && not brace(c) &&
                    not digit(c) && not brace(t.text) &&
                    not space(c);
                else
                if (t.kind == "literal") same =
                    t.text == "\"" || not
                    t.text.ends_with("\"") ||
                    t.text.ends_with("\\\"");

                if (same)
                {
                    t.text += c;
                    t.range.upto.offset++;

                    if (t.text == ";;")
                        t.kind = "comment";
                }
                else
                {
                    if (t.text == "->") t.text = (char*)(u8"→");

                    if (t.text != "") tokens += t;

                    t = token {c, "", range{
                        {n, offset},
                        {n, offset}}
                    };
                    t.kind =
                        alpha (c) ? "name"    :
                        digit (c) ? "number"  :
                        space (c) ? "space"   : 
                        c == "\"" ? "literal" : 
                                    "symbol"  ;
                }
            }

            if (t.text == "->") t.text = (char*)(u8"→");

            if (t.text != "") tokens += t;

            tokens += token {"\n", "", range{
                {n, line.size()},
                {n, line.size()}}
            };

            t = token{};
        }

        return tokens;
    }
}

