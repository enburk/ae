#pragma once
#include "doc.h"
namespace doc::lexica::ae
{
    inline array<token> parse (const text & text)
    {
        array<token> tokens; token t;

        for (auto [line, n] : text.lines.whole())
        {
            for (auto [c, offset] : line.whole())
            {
                bool same = false;

                if (t.kind == "comment") same = true; else
                if (t.kind == "space"  ) same = space (c); else
                if (t.kind == "name"   ) same = alpha (c) || digit (c) || c == "."; else
                if (t.kind == "number" ) same = alpha (c) || digit (c) || c == "."; else

                if (t.kind == "literal") same =
                    t.text == "\"" || not
                    t.text.ends_with("\"") ||
                    t.text.ends_with("\\\""); else

                if (t.kind == "symbol" ) same = false
                
                ||  t.text == "+" && (c == "=" || c == "+")
                ||  t.text == "-" && (c == "=" || c == "-" || c == ">")
                ||  t.text == "/" && (c == "=" || c == "/")
                ||  t.text == "*" && (c == "=" || c == "*")

                ||  t.text == "<" && (c == "=" || c == "<")
                ||  t.text == ">" && (c == "=" || c == ">")
                ||  t.text == "=" && (c == "=")
                ||  t.text == ":" && (c == ":")

                ||  t.text =="<<" && (c == "<")
                ||  t.text ==">>" && (c == ">")

                ||  t.text == "." && (c == ".")
                ||  t.text ==".." && (c == ".")
                ;
                if (same)
                {
                    t.text += c;
                    t.range.upto.offset++;

                    if (t.text == ";;")
                        t.kind = "comment";
                }
                else
                {
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

