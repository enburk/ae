#pragma once
#include "sys.h"
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget.h"
#include "gui_widget_layout.h"
namespace gui
{
    struct text final : widget<text>
    {
        struct glyph final : widget<glyph>
        {
            glyph () = default;
            glyph (sys::glyph g) : g(g) {
                advance = XY(g.advance, 0);
                baseline = XY(0, g.ascent);
                resize(g.size);
            }
            unary_property<sys::glyph> g;

            Opacity opacity () override { return semitransparent; }

            void on_change (void* what) override {
                if (what == &g) {
                    advance = XY(g.now.advance, 0);
                    baseline = XY(0, g.now.ascent);
                    resize(g.now.size);
                    update();
                }
            }
            void on_render (Frame<RGBA> frame, XY offset, uint8_t alpha) override {
                sys::render(g.now, frame, offset, alpha, coord.now.x);
            }
        };

        struct line final : widget<line>
        {
            widgetarium<canvas> canvases;
            layout<glyph, horizontal> glyphs;

            int size () const { return glyphs.size(); }

            void append (sys::glyph g) {
                 glyphs.emplace_back(std::move(g));
                 resize(XY(coord.now.size.x, glyphs.coord.now.size.y));
            }
            void append (sys::token token) {
                 for (auto && g : token.glyphs)
                    append (std::move(g));
            }
            void replace(int pos, sys::glyph g) {
                 erase (pos);
                 insert(pos, std::move(g));
            }
            void insert (int pos, sys::glyph g) {
                 append (std::move(g)); glyphs.rotate(pos,
                    size()-1,
                    size());
            }
            void insert (int pos, sys::token token) {
                 append (std::move(token)); glyphs.rotate(pos,
                    size()-token.glyphs.size(),
                    size());
            }
            void erase (int pos, int num = 1) {
                 glyphs.erase(pos, num);
                 resize(XY(coord.now.size.x, glyphs.coord.now.size.y));
            }

            void on_change (void* what) override {
                if (what == &coord) canvases.resize(coord.now.size);
            }
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

        struct caret final : widget<caret>
        {
            canvas canvas;
            property<int> position;
            property<bool> insert_mode;
            void on_change (void* what) override
            {
            }
        };

        struct editor final : widget<editor>
        {
            widgetarium<caret> carets;

            //void set_caret(int position) { carets.clear(); add_caret(position); }
            //void add_caret(int position) { int n = size();
            //     auto i = std::find_if (carets.begin(), carets.end(),
            //         [position](auto & caret){ return caret.position.now <= position; })
            //         - carets.begin(); // linear insertion sort by decreasing positions...
            //     if (i < n && carets(i).position.now == position) return;
            //     carets.emplace_back().position = position;
            //     carets.rotate (i, n, n+1); 
            //}
            //
            //void insert (sys::glyph g) {
            //     for (auto & caret : carets) {
            //        insert(caret.position.now, g);
            //        caret.position = caret.position.now + 1;
            //     }
            //}
            //void insert (sys::token token) {
            //     for (auto & caret : carets) {
            //        insert(caret.position.now, token);
            //        caret.position = caret.position.now + token.glyphs.size();
            //     }
            //}

            void erase () {}
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