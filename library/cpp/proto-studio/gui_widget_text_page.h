#pragma once
#include "../sys.h"
#include "doc.h"
#include "gui_widget.h"
#include "gui_widgetarium.h"
#include "gui_widget_text_aux.h"
namespace gui::text
{
    struct format
    {
        int width  = max<int>();
        int height = max<int>();

        XY alignment = XY{center, center};

        bool word_wrap = true;
        bool ellipsis = true;

        struct limits { int margin = 0; XY obstacle; };
        limits left, right;
    };

    struct lexeme { str text; sys::glyph_style_index style; };

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

        void fill (array<lexeme>::range lexemes, format format)
        {
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
                auto style_index = sys::glyph_style_index(style);

                token.fill(t.text, style_index);

                if (rows.back().coord.size.x +
                    rows.back().advance + token.width > width)
                    rows += row();

                auto & r = rows.back();
                r.ascent  = max (r.ascent,  token.ascent);
                r.descent = max (r.descent, token.descent);
                r.offsets += r.coord.size.x + r.advance;
                r.advance = token.advance;
                r.coord.size.y = max (r.coord.size.y, r.ascent + r.descent);
                r.coord.size.x = r.offsets.back() + token.width;
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
                    token.ascent));
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