#pragma once
#include "doc.h"
#include "doc_lexica.h"
namespace doc::lexica
{
/// template<class I,
/// std::enable_if_t<std::is_same_v<typename
/// std::iterator_traits<I>::value_type, char>>>
/// inline array<Token> txt (range<I> text)
    inline array<Token> txt (Range<char> text)
    {
        array<Token> output; Token t; t.kind = "space";

        for (auto [c, n] : text + '\n')
        {
            bool same_kind = true;

            if (c      == '\n'   ) same_kind = false; else
            if (t.text == "\n"   ) same_kind = false; else
            if (t.kind == "text" ) same_kind =!space (c); else
            if (t.kind == "space") same_kind = space (c);

            if (same_kind) t.text += c; else
            {
                if (t.text != "")
                    output += t;

                t.begin = n;
                t.text  = c;
                t.kind  = c == '\n' ? "eoln" : space (c) ? "space" : "text";
            }
        }

        return output;
    }

    inline array<Token> txt (str text)
    {
        return txt (array<char>(text.begin(), text.end()).range());
    }
}

namespace doc::html
{
    inline str encoded (str text) { return text; }
}