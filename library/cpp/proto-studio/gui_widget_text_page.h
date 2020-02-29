#pragma once
#include "../sys.h"
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget.h"
#include "gui_widgetarium.h"
#include "gui_widget_layout.h"
namespace gui::text
{
    struct glyph:
    widget<glyph>
    {
        unary_property<sys::glyph> Glyph;

        Opacity opacity () override { return semitransparent; }

        void on_change (void* what) override { if (what != &Glyph) return;
            advance = XY(Glyph.now.advance, 0);
            baseline = XY(0, Glyph.now.ascent);
            resize(Glyph.now.size);
            update();
        }
        void on_render (pix::frame<RGBA> frame, XY offset, uint8_t alpha) override {
            sys::render(Glyph.now, frame, offset, alpha, coord.now.x);
        }
    };

    struct token:
    widget<token>
    {
        doc::token doc_token;

        unary_property<sys::token> Token;
        
        widgetarium<glyph> glyphs;

        void on_change (void* what) override { if (what != &Token) return;
            for (int i = 0; i < Token.now.glyphs.size(); i++) {
                const auto & Glyph = Token.now.glyphs[i];
                glyph & glyph = i < glyphs.size() ? glyphs(i) : glyphs.emplace_back();
                glyph.Glyph = Glyph;
                glyph.move_to(Token.now.offsets[i]);
            }
            glyphs.resize(Token.now.size);
            resize(Token.now.size);
        }
    };

    ///////////////////////////////////////////////////////////////////////
    const int                   top    =-1,
    justify_left =-2, left =-1, center = 0, right = 1, justify_right = 2,
                                bottom = 1;
    ///////////////////////////////////////////////////////////////////////

    struct line final : widgetarium<token>
    {
        struct row
        {
            XYWH coord;
            int ascent  = 0;
            int descent = 0;
            int advance = 0;
            array<int> offsets;
        };
        array<row> rows;

        void fill (str text, sys::glyph_style style) {
             std::map<str, sys::glyph_style> styles;
             array<doc::token> tokens = doc::lexica::txt(text);
             fill (max<int>(), left, false, tokens.whole(), styles, style);
        }
        void fill (
            int width,
            int align,
            bool word_wrap,
            array<doc::token>::range tokens,
            const std::map<str, sys::glyph_style> & styles,
            const sys::glyph_style & default_style)
        {
            rows.clear(); if (width == 0) { resize(XY()); return; }
            rows += row();

            for(auto [t, n] : tokens)
            {
                token & token = n < size() ? (*this)(n) : emplace_back();

                sys::glyph_style style;
                auto it = styles.find(t.kind);
                style = it != styles.end() ? it->second : default_style;

                auto& Token = token.Token.now;
              //if (Token.text != t.text || Token.style() != style)
                if (token.doc_token.text != t.text ||
                    token.doc_token.kind != t.kind) {
                    token.doc_token  = t;
                    token.Token = sys::token(t.text, style);
                }
                token.doc_token.place = t.place;

                if (rows.back().coord.size.x +
                    rows.back().advance + Token.size.x > width)
                    rows += row();

                auto & r = rows.back();
                r.ascent  = max (r.ascent,  Token.ascent);
                r.descent = max (r.descent, Token.descent);
                r.offsets += r.coord.size.x + r.advance;
                r.advance = Token.advance;
                r.coord.size.y = max (r.coord.size.y, r.ascent + r.descent);
                r.coord.size.x = r.offsets.back() + Token.size.x;
            }
            int h = 0; for (auto & r : rows) { r.coord.y = h; h += r.coord.size.y; }
            int n = 0; for (auto & r : rows) {
            int c = 0; for (int offset : r.offsets)
            {
                int shift = 0;
                switch (align) {
                case left  : break;
                case center: shift = width/2 - r.coord.size.x/2; break;
                case right : shift = width -   r.coord.size.x; break;
                case justify_left: case justify_right:
                    shift = width / r.offsets.size() +
                            width % r.offsets.size() * c; break;
                }
                c++;
                if (c == rows.size()) {
                    if (align == justify_left) shift = 0; else
                    if (align == justify_right) shift = width - r.coord.size.x;
                }

                token & token = (*this)(n++);
                token.move_to(XY(
                    r.coord.x + offset + shift,
                    r.coord.y + r.ascent -
                    token.Token.now.ascent));
            }}
            truncate(n);
            resize(XY(width, h));
        }
    };

    ///////////////////////////////////////////////////////////////////////
            
    struct page final : widgetarium<line>
    {
        void fill (
            int width,
            int align,
            bool word_wrap,
            const array<doc::token> & tokens,
            const std::map<str, sys::glyph_style> & styles,
            const sys::glyph_style & default_style)
        {
            if (width == 0) return;
            int l = 0;
            /*
            for (auto i = tokens.begin(); i != tokens.end(); ) {
                 auto j = std::find_if(i, tokens.end(), [](auto t){ return t.text == "\n"; });
                 if (j != tokens.end()) j++;
                 if (l >= size()) emplace_back();
                 (*this)(l++).fill(
                     width, align, word_wrap,
                     array<doc::token>(i, j).whole(),
                     styles, default_style);
                 i = j;
            }
            */
            for (auto i = tokens.begin(); i != tokens.end(); ) {
                 auto j = std::find_if(i, tokens.end(), [](auto t){ return t.text == "\n"; });
                 if (j != tokens.end()) j++;
                 if (l >= size()) emplace_back();
                 (*this)(l++).fill(
                     width, align, word_wrap,
                     array<doc::token>(std::vector<doc::token>(i, j)).whole(),
                     styles, default_style);
                 i = j;
            }
            truncate(l);
            int h = 0;
            for (auto & line : *this) {
                line.move_to(XY(0, h));
                h += line.coord.now.size.y;
            }
            resize(XY(width,h));
        }
    };
} 