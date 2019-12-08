#pragma once
#include "doc_lexica_txt.h"
#include "gui_widget_text_page.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view final : widget<view>
    {
        canvas canvas; page page;

        binary_property<str> text;
        binary_property<str> html;

        property<RGBA> color;
        binary_property<sys::glyph_style> style;

        binary_property<bool> word_wrap = true;
        binary_property<XY> alignment = XY{center, center};

        array<doc::Token> tokens;
        std::map<str, sys::glyph_style> styles;

        void refresh ()
        {
            //update(); // ???
            page.fill (
                coord.now.size.x,
                alignment.now.x,
                word_wrap.now,
                tokens, styles, style.now);

            //if (alignment.now == align<horizontal>::center)
                page.move_to(coord.now.size/2 - page.coord.now.size/2);
        }

        void on_change (void* what) override
        {
            if (what == &text)
            {
                tokens = doc::lexica::txt(text.now);
                styles.clear();

                html.was = html.now; html.now = "";
                for (auto && token : tokens)
                html.now += doc::html::encoded(token.text);

                refresh();
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
                    style = sys::glyph_style{ sys::font{"", metrics::text::height}, pix::black };
                refresh();
            }
            if (what == &color)
            {
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
            if (what == &word_wrap)
            {
                refresh();
            }
            //if (what == &align)
            //{
            //}
        }
    };
} 