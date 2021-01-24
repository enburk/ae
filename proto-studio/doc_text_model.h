#pragma once
#include "doc_text_model_b.h"
namespace doc::text
{
    struct model : b::model, polymorphic
    {
        using base = b::model;

        array<token> tokens; report log;

        virtual void tokenize ()
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

        virtual bool tokenize_if (bool updated)
        {
            if (updated) tokenize();
            return updated;
        };

        virtual bool undo        () { return tokenize_if(base::undo     ()); }
        virtual bool redo        () { return tokenize_if(base::redo     ()); }
        virtual bool erase       () { return tokenize_if(base::erase    ()); }
        virtual bool backspace   () { return tokenize_if(base::backspace()); }
        virtual bool insert (str s) { return tokenize_if(base::insert  (s)); }
        virtual bool set   (text t) { return tokenize_if(base::set     (t)); }
    };
}