#pragma once
#include "gui_widget_text_column.h"
#include "gui_widget_text_model.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view:
    widget<view>
    {
        canvas canvas;
        column column;
        unary_property<str> text;
        unary_property<str> html;

        property<RGBA> color;
        binary_property<sys::font> font;
        binary_property<glyph_style> style;
        binary_property<bool> word_wrap = true;
        binary_property<bool> ellipsis = false;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> margin_right;
        binary_property<XY> margin_left;
        binary_property<XY> shift;

        htmlmodel model; array<doc::entity> entities;

        view () { on_change(&skin); }

        void refresh ()
        {
            format format;
            format.width = coord.now.size.x;
            format.margin_right = margin_right.now;
            format.margin_left = margin_left.now;
            format.alignment = alignment.now;
            format.word_wrap = word_wrap.now;
            format.ellipsis = ellipsis.now; if (ellipsis.now)
            format.height = coord.now.size.y;

            model = htmlmodel(entities, glyph_style_index(style.now), format);
            column.fill (model.sections);
            align();
        }

        void align ()
        {
            int H = coord.now.size.y;
            int h = column.coord.now.size.y;

            column.move_to(XY(
                shift.now.x,
                shift.now.y + (
                alignment.now.y == center ? H/2 - h/2 :
                alignment.now.y == bottom ? H   - h   :
                0)));
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                canvas.coord = coord.now.local();
                refresh();
            }
            if (what == &text)
            {
                entities.clear();
                entities += doc::entity{"", "text", ""};
                entities.back().head = doc::lexica::txt::parse(doc::text(text.now));
                html.now = doc::lexica::html::encoded(text.now);
                refresh();
            }
            if (what == &html)
            {
                text.now = doc::lexica::html::untagged(html.now);
                entities = doc::syntax::html::combine(
                           doc::syntax::html::parse(
                           doc::lexica::html::parse(doc::text(html.now))));
                if (false) { // debug
                    auto tokens = doc::syntax::html::print(entities);
                    entities.clear();
                    entities += doc::entity{"", "text", ""};
                    entities.back().head = tokens;
                }
                refresh();
            }
            if (what == &skin)
            {
                style = sys::glyph_style
                {
                    skins[skin.now].font,
                    skins[skin.now].normal.fore_color
                };
            }
            if (what == &font)
            {
                style.was = style.now;
                style.now.font = font.now;
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
                font.was = font.now;
                font.now = style.now.font;
                color.was = color.now;
                color.now = style.now.color;
                refresh();
            }
            if (what == &word_wrap
            ||  what == &margin_right
            ||  what == &margin_left
            ||  what == &alignment)
            {
                refresh();
            }
            if (what == &shift)
            {
                align();
            }
        }
    };
} 