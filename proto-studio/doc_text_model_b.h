#pragma once
#include <functional>
#include "doc_text_model_a.h"
namespace doc::text
{
    struct model : a::model
    {
        using base = a::model;

        explicit model (str s = "") : base(s) {}

        array<token> tokens;

        std::function<void()> tokenize = [this]() mutable
        {
            tokens.clear(); token t;

            for (auto [n, line] : lines.enumerate())
            {
                for (auto [offset, glyph] : line.enumerate())
                {
				    if (!glyph.letter() && !glyph.digit())
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

                if (n != lines.size()-1)
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
        };

        void set (text const& text)
        {
            *this = model(text.string());
            tokenize();
            ///////////// ...
        }

        bool undo        () { if (!base::undo     ()) return false; tokenize(); return true; }
        bool redo        () { if (!base::redo     ()) return false; tokenize(); return true; }
        bool erase       () { if (!base::erase    ()) return false; tokenize(); return true; }
        bool backspace   () { if (!base::backspace()) return false; tokenize(); return true; }
        bool insert (str s) { if (!base::insert  (s)) return false; tokenize(); return true; }
    };
}