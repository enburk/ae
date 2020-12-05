#pragma once
#include "aux_utils.h"
#include "aux_string.h"
#include "doc_text_layout.h"
namespace doc::text
{
    using namespace aux;

    /*
    struct glyph
    {
        char cc[4] = {};
        enum kind { letter, digit, space, symbol };
        kind kind = kind::symbol;

        str string () {
            str s; for (char c : cc)
            if (c) s += c; else break;
            return s;
        }

        static array<glyph> parse (const str & s)
        {
            array<glyph> glyphs;
            glyphs.reserve(s.size());

            auto check = [&s](auto i){ if (i == s.end() ||
                (static_cast<uint8_t>(*i) & 0b11000000) != 0b10000000 )
                    throw std::runtime_error("unicode: broken UTF-8"); };

            for (auto i = s.begin(); i != s.end(); )
            {
                glyph g; int n = 0;
                char c = *i++; g.cc[n++] = c;
                auto u = static_cast<uint8_t>(c);

                if ((u & 0b11000000) == 0b11000000) { check(i); g.cc[n++] = *i++; // 110xxxxx 10xxxxxx
                if ((u & 0b11100000) == 0b11100000) { check(i); g.cc[n++] = *i++; // 1110xxxx 10xxxxxx 10xxxxxx
                if ((u & 0b11110000) == 0b11110000) { check(i); g.cc[n++] = *i++; // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                }}}
                
                glyphs += g;
            }

            return glyphs;
        }
    };
    */

    inline constexpr auto ascii (char c) { return (c >= ' ' && c <= '~'); };
    inline constexpr auto space (char c) { return (c == ' ' || c =='\t'); };
    inline constexpr auto digit (char c) { return (c >= '0' && c <= '9'); };
    inline constexpr auto alpha (char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); };

    inline array<str> symbols
    {
        "\xE2" "\x80" "\xA6", // …
        "\xE2" "\x80" "\x94", // m-dash
        "\xE2" "\x80" "\x93", // n-dash
        "\xE2" "\x80" "\x99", // ’
        "\xE2" "\x80" "\x9C", // “
        "\xE2" "\x80" "\x9D", // ”
        "\xC2" "\xAB", // «
        "\xC2" "\xBB", // »
    };

    inline array<token> parse (const text & text)
    {
        array<token> tokens; token t;

        for (auto [line, n] : text.lines.whole())
        {
            for (auto [glyph, offset] : line.whole())
            {
                if((glyph.size() == 1 && !digit(glyph[0]) && !alpha(glyph[0]))
                || (glyph.size() >= 2 && symbols.contains(glyph)))
                {
                    if (t.text != "") tokens += t;

                    tokens += token {glyph, "", range{
                        {n, offset},
                        {n, offset+1}}
                    };

                    t = token {"", "", range{
                        {n, offset+1},
                        {n, offset+1}}
                    };
                }
                else
                {
                    t += glyph;
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
