#pragma once
#include "doc.h"
namespace doc::lexica::txt
{
    inline array<token> parse (const text & text)
    {
        array<token> tokens; token t;

        for (auto [line, n] : text.lines.whole())
        {
            for (auto [glyph, offset] : line.whole())
            {
                if (glyph == " ")
                {
                    if (t.text != "") tokens += t;

                    tokens += token {" ", "", range{
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
