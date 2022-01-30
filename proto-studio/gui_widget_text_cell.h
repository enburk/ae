#pragma once
#include "doc_html_model.h"
#include "gui_widget_text_column.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct cell:
    widget<cell>
    {
        column column;

        unary_property<str> text;
        unary_property<str> html;

        property<RGBA> color;
        binary_property<font> font;
        binary_property<style> style;
        binary_property<padding> lpadding;
        binary_property<padding> rpadding;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> shift;
        binary_property<bool> wordwrap = true;
        binary_property<bool> ellipsis = false;
        property<bool> refresh = false;

        doc::html::model xmodel;
        doc::view::model* model = &xmodel;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                refresh.go(true, time(1));
            }
            if (what == &text)
            {
                xmodel.text(text.now);
                html.now = xmodel.html();
                refresh.go(true, time(1));
            }
            if (what == &html)
            {
                xmodel.html(html.now);
                text.now = xmodel.text();
                refresh.go(true, time(1));
            }
            if (what == &skin)
            {
                style = pix::text::style{
                skins[skin.now].font,
                skins[skin.now].light.second};
            }
            if (what == &font)
            {
                style.was = style.now;
                style.now.font = font.now;
                refresh.go(true, time(1));
            }
            if (what == &color)
            {
                style.was = style.now;
                style.now.color = color.now;
                refresh.go(true, time(1));
            }
            if (what == &style)
            {
                font.was = font.now;
                font.now = style.now.font;
                color.was = color.now;
                color.now = style.now.color;
                refresh.go(true, time(1));
            }
            if (what == &wordwrap
            or  what == &ellipsis
            or  what == &lpadding
            or  what == &rpadding
            or  what == &alignment)
            {
                refresh.go(true, time(1));
            }
            if (what == &refresh and refresh.now)
            {
                refresh.now = false;

                format format;
                format.alignment = alignment.now;
                format.lpadding = lpadding.now;
                format.rpadding = rpadding.now;
                format.wordwrap = wordwrap.now;
                format.ellipsis = ellipsis.now;
                format.width = coord.now.size.x; if (ellipsis.now)
                    format.height = coord.now.size.y;

                model->set(style.now, format);
                column.fill(model->lines);
            }
            if (what == &refresh
            or  what == &shift)
            {
                int H = coord.now.size.y;
                int h = column.coord.now.size.y;

                column.move_to(XY(0, H > h and
                    alignment.now.y == center ? H/2 - h/2 :
                    alignment.now.y == bottom ? H   - h   :
                    0) + shift.now);
            }

            notify(what);
        }

        generator<XYWH> bars(range range) {
            for (XYWH bar: column.bars(range))
                co_yield bar +
                column.coord.now.origin +
                coord.now.origin; }

        XYWH bar(place place, bool virtual_space) {
            return column.bar(place, virtual_space) +
                column.coord.now.origin +
                coord.now.origin; }
    };
} 