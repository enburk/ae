#pragma once
#include "doc_lexica_txt.h"
#include "gui_widget_text_page.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view final : widget<view>
    {
        canvas canvas; page page;

        unary_property<str> text;
        unary_property<str> html;
        unary_property<array<doc::Token>> tokens;

        property<RGBA> color;
        binary_property<sys::glyph_style> style;
        binary_property<bool> word_wrap = true;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> shift;

        std::map<str, sys::glyph_style> styles;

        void refresh ()
        {
            page.fill (
                coord.now.size.x,
                alignment.now.x,
                word_wrap.now,
                tokens.now, styles, style.now);

            page.move_to(XY(
                shift.now.x,
                shift.now.y + (
                alignment.now.y == center ? coord.now.size.y/2 - page.coord.now.size.y/2 :
                alignment.now.y == bottom ? coord.now.size.y   - page.coord.now.size.y   :
                0)));
        }

        void on_change (void* what) override
        {
            if (what == &tokens) { refresh(); }
            if (what == &text)
            {
                styles.clear();
                tokens = doc::lexica::txt(text.now);
                html.now = ""; for (const auto & token : tokens.now)
                html.now += doc::html::encoded(token.text);

            }
            if (what == &html)
            {
            //  page.clear();
            //  text.was = text.now;
            //  for (auto && token : browser(
            //      doc::translator::html(html))) {
            //      text.now += token.text;
            //      page.append(token);
            //  }
            }
            if (what == &coord && coord.was.size != coord.now.size)
            {
                canvas.coord = coord.now.local();
                if (style.now == sys::glyph_style())
                    style = sys::glyph_style{ schemas[""].font, schemas[""].black };
                refresh();
            }
            if (what == &color)
            {
                if (style.now == sys::glyph_style())
                    style = sys::glyph_style{ schemas[""].font, schemas[""].black };
                style.was = style.now;
                style.now.color = color.now;
                refresh();
            }
            if (what == &style)
            {
                color.was = color.now;
                color.now = style.now.color;
                refresh();
            }
            if (what == &word_wrap
            ||  what == &alignment
            ||  what == &shift)
            {
                refresh();
            }
        }
    };
} 