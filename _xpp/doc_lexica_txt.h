#pragma once
#include "doc.h"
#include "doc_lexica.h"
namespace lexica
{
//  template<class I,
//  std::enable_if_t<std::is_same_v<typename
//  std::iterator_traits<I>::value_type, char>>>
//  array<Token> txt (range<I> text)
    array<Token> txt (Range<char> text)
    {
        array<Token> output; Token t;

        for (auto [c, n] : text + '\n')
        {
            bool same = c != '\n';

            if (t.kind == "space") same = space (c);

            if (same) t.text += c; else
            {
                if (t.text != "")
                    output += t;

                t.begin = n;
                t.text  = c;
                t.kind  =
                    c == '\n' ? "end of line" :
                    space (c) ? "space" : "text";
        }

        return output;
    }
}