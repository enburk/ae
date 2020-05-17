#pragma once
#include "doc.h"
namespace doc::lexica::cpp
{
    inline constexpr auto ascii (char c) { return c >= ' ' && c <= '~'; }
    inline constexpr auto space (char c) { return c == ' ' || c =='\t'; }
    inline constexpr auto digit (char c) { return c >= '0' && c <= '9'; }
    inline constexpr auto alpha (char c) { return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z'; }

    inline auto ascii (str c) { return c.size() == 1 && ascii(c[0]); }
    inline auto space (str c) { return c.size() == 1 && space(c[0]); }
    inline auto digit (str c) { return c.size() == 1 && digit(c[0]); }
    inline auto alpha (str c) { return c.size() == 1 && alpha(c[0]); }

    inline array<token> parse (const text & text)
    {
        array<token> tokens; token t; str comment;

        for (auto [line, n] : text.lines.whole())
        {
            if (comment == "//") comment = "";
            if (comment == "/**") comment = "/*";

            for (auto [c, offset] : line.whole())
            {
                if (comment != "") { t.kind = "comment"; t.text += c; }

                if (comment == "/*"  && c != "*") {                  continue; }
                if (comment == "/*"  && c == "*") { comment = "/**"; continue; }
                if (comment == "/**" && c != "/") { comment = "/*";  continue; }
                if (comment == "/**" && c == "/") { comment = "";    continue; }

                if (comment == "//") continue;

                bool same = false;

                if (t.kind == "space"  ) same = space (c); else
                if (t.kind == "name"   ) same = alpha (c) || digit (c) || c == "_"; else
                if (t.kind == "number" ) same = alpha (c) || digit (c) || c == "."; else
                if (t.kind == "literal") same = t.text == "\"" || !t.text.ends_with("\"") || t.text.ends_with("\\\""); else
                if (t.kind == "char"   ) same = t.text == "\'" || !t.text.ends_with("\'") || t.text.ends_with("\\\'"); else
                if (t.kind == "symbol" ) same = false
                
                ||  t.text == "+" && ( c == "=" || c == "+" )
                ||  t.text == "-" && ( c == "=" || c == "-" || c == ">" )
                ||  t.text == "/" && ( c == "=" || c == "*" || c == "/" )
                ||  t.text == "*" && ( c == "=" )

                ||  t.text == "<" && ( c == "=" || c == "<" )
                ||  t.text == ">" && ( c == "=" || c == ">" )
                ||  t.text == "&" && ( c == "=" || c == "&" )
                ||  t.text == "|" && ( c == "=" || c == "|" )
                ||  t.text == "!" && ( c == "=" )
                ||  t.text == "=" && ( c == "=" )
                ||  t.text == ":" && ( c == ":" )

                ||  t.text =="<<" && ( c == "=" )
                ||  t.text ==">>" && ( c == "=" )

                ||  t.text == "." && ( c == "." )
                ||  t.text ==".." && ( c == "." )
                ;
                if (same)
                {
                    t.text += c;
                    if (t.text == "//" ||
                        t.text == "/*") {
                        t.kind = "comment";
                        comment = t.text;
                    }
                }
                else
                {
                    if (t.text != "") tokens += t;

                    t = token {c, "", range{
                        {n, offset+1},
                        {n, offset+1}}
                    };
                    t.kind =
                        alpha (c) ? "name"    :
                        digit (c) ? "number"  :
                        space (c) ? "space"   : 
                        c == "\"" ? "literal" : 
                        c == "\'" ? "char"    : 
                                    "symbol"  ;
                }
            }

            if (t.text != "") tokens += t;

            if (n != text.lines.size()-1)
            {
                tokens += token {"\n", "", range{
                    {n, line.size()},
                    {n, line.size()}}
                };

                t = token {"", "", range{
                    {n+1, 0},
                    {n+1, 0}}
                };
            }
        }

        return tokens;
    }
}

