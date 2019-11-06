#pragma once
#include "sys.h"
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget.h"
namespace gui
{
    struct text final : widget<text>
    {
        struct glyph final : widget<glyph>
        {
            unary_property<sys::glyph> g;

            Opacity opacity () override { return semitransparent; }

            void on_change (void* what) override
            {
                if (what == &g) { resize(g.now.size); update(); }
            }
            void on_render (Frame<RGBA> frame, XY offset, uint8_t alpha) override
            {
                sys::render(g.now, frame, offset, alpha, coord.now.x);
            }
        };

        struct line final : widget<line> // layout<glyph, horizontal> //
        {
            widgetarium<canvas> canvases;
            widgetarium<glyph> glyphs;
            int minimum_height = 0;
            int baseline = 0;

            int advance (int f, int l) { --l; return f > l ? 0 :
                glyphs(l).coord.now.x -
                glyphs(f).coord.now.x +
                glyphs(l).g.now.advance;
            }

            void shift (int f, int l, XY d) {
                for (int i=f; i<l; i++)
                    glyphs(i).shift(d);
            }

            void append (sys::glyph g)
            {
                int x = advance(0, glyphs.size());
                auto & glyph = glyphs.emplace_back();
                glyph.g = g;
                glyph.shift(XY(x, baseline - g.ascent));
                if (baseline < g.ascent) { shift(0, glyphs.size(), XY(0,
                   -baseline + g.ascent));
                    baseline = g.ascent; }
                x += g.size.x;
                glyphs.
                resize(XY(x, max(coord.now.size.y, g.size.y)));
                resize(XY(x, max(coord.now.size.y, g.size.y)));
            }
            void append (sys::token token)
            {
                for (auto & g : token.glyphs)
                    append (g);
            }
            void insert (int pos, sys::glyph g) {
                append(std::move(g)); rotate(pos,
                    glyphs.size()-1,
                    glyphs.size());
            }
            void insert (int pos, sys::token token) {
                append(std::move(token)); rotate(pos,
                    glyphs.size()-token.glyphs.size(),
                    glyphs.size());
            }
            int rotate (int f, int m, int l)
            {
                int a1 = advance(f,m);
                int a2 = advance(m,l);
                int n = glyphs.rotate(f, m, l);
                shift(f,n, XY(-a1, 0));
                shift(n,l, XY( a2, 0));
            }
            void erase (int pos, int num = 1)
            {
                truncate(rotate(pos, pos+num, glyphs.size()));
                int height = baseline = 0;
                for (auto & g : glyphs) {
                    baseline = max (baseline, g.g.now.ascent);
                    height =   max (height,   g.g.now.size.y);
                }
                height = max (height, minimum_height);
                for (auto & g : glyphs) g.move_to(XY(g.coord.now.x, baseline - g.g.now.ascent));
                glyphs.
                resize(XY(glyphs.coord.now.size.x, height));
                resize(XY(glyphs.coord.now.size.x, height));
            }
            void truncate (int pos)
            {
                while (glyphs.size() > pos)
                    erase(glyphs.size()-1);
            }


            struct selection { RGBA fore, back; int begin, end; };

            array<selection> selections;

            //array<caret> carets;
        };

        struct page final : widget<page>
        {
            widgetarium<line> lines;
            property<int> indent = 0;

            //void insert (pix::Token token, int pos)
            //{
            //    glyphs.insert(glyphs.begin()+pos, token.glyphs.begin(), token.glyphs.end());
            //}
        };


        std::map<str, sys::glyph_style> styles;



        typedef doc::Document Document;

        struct Context
        {
            Document* document; int top = 0;
        };

        Document internal_document;
        Context  internal_context {&internal_document};
        Context* context = &internal_context;

        //Text (base::widget* parent = nullptr) : widget (parent) {}

        //array<line> lines;

        void operator = (str s)
        {
        }

        void insert (str text) { insert (std::move(text), context->document->text.size()); }
        void insert (str text, int pos)
        {
            context->document->insert(text, pos);

            for (auto token : context->document->syntax)
            {

            }
        }

        void remove (int pos)
        {
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                //auto r = coord.now.local();
                //auto h = r.h / 
                //text       .move_to(r);
            }
        }
    };
} 