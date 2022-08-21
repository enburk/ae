#pragma once
#include "doc_text_text.h"
namespace doc::ae::lexica
{
	using text::token;

    array<str> subsup = {
    u8"₀", u8"₁", u8"₂", u8"₃", u8"₄", u8"₅", u8"₆", u8"₇", u8"₈", u8"₉", 
    u8"⁰", u8"¹", u8"²", u8"³", u8"⁴", u8"⁵", u8"⁶", u8"⁷", u8"⁸", u8"⁹"};

    bool ascii (char c) { return (c >= ' ' && c <= '~'); }
    bool space (char c) { return (c == ' ' || c =='\t'); }
    bool digit (char c) { return (c >= '0' && c <= '9'); }
    bool alpha (char c) { return (c >= 'A' && c <= 'Z') ||
                                 (c >= 'a' && c <= 'z'); }

    bool international (str s)
    {
        return false;
    }

    bool ascii (str c) { return c.size() == 1 && ascii(c[0]); }
    bool space (str c) { return c.size() == 1 && space(c[0]); }
    bool digit (str c) { return c.size() == 1 && digit(c[0]) or subsup.contains(c); }
    bool alpha (str c) { return c.size() == 1 && alpha(c[0]) or international(c); }
    bool brace (str c) { return
    c == "{" or c == "(" or c == "[" or
    c == "}" or c == ")" or c == "]"; }

    array<token> parse (const text::text & text)
    {
        array<token> tokens; token t;

        for (auto [n, line] : enumerate(text.lines))
        {
            for (auto [offset, c] : enumerate(line))
            {
                if (c == "\r") continue;

                bool same = false;

                if (t.kind == "comment"  ) same = true; else
                if (t.kind == "space"    ) same = space(c); else
                if (t.kind == "name"     ) same = alpha(c) || digit(c) || c == "."; else
                if (t.kind == "number"   ) same = alpha(c) || digit(c) || c == "."; else
                if (t.kind == "semicolon") same = c == ";"; else
                if (t.kind == "symbol"   ) same =
                    not alpha(c) and not brace(c) and
                    not digit(c) and not brace(t.text) and
                    not space(c) and
                    c != ","     and
                    c != ";"     and
                    c != "\n"    and
                    c != "\"";
                else
                if (t.kind == "literal") same =
                    t.text == "\"" or not
                    t.text.ends_with("\"") or
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
                    if (t.text != "") tokens += t;

                    t = token {c, "", "", range{
                        {n, offset},
                        {n, offset}}};

                    t.kind =
                        alpha (c) ? "name" :
                        digit (c) ? "number" :
                        space (c) ? "space" : 
                        c == "\"" ? "literal" : 
                        c == ";"  ? "semicolon" : 
                                    "symbol";
                }
            }

            // line termination,
            // be aware of comments

            if (t.text != "") tokens += t;

            if (n != text.lines.size()-1)
            {
                tokens += token {"\n", "", "", range{
                    {n, line.size()},
                    {n, line.size()}}};

                t = token{};
            }
        }

        return tokens;
    }
}

