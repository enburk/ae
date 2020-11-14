#pragma once
#include "doc.h"
namespace doc::lexica::txt
{
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
