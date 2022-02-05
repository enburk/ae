#pragma once
#include "doc_html_model.h"
#include "gui_widget_text_cell.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view:
    widget<view>
    {
        canvas canvas; cell cell;
        frame current_line_frame;
        frame frame;

        unary_property<str> text;
        unary_property<str> html;

        property<RGBA> color;
        binary_property<font> font;
        binary_property<style> style;
        binary_property<int> lpadding;
        binary_property<int> rpadding;
        binary_property<array<XY>> lwrap;
        binary_property<array<XY>> rwrap;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> shift;
        binary_property<bool> wordwrap = true;
        binary_property<bool> ellipsis = false;
        property<bool> refresh = false;

        doc::html::model xmodel;
        doc::view::model* model = &xmodel;

        lines& lines = cell.lines;
        unary_property<array<range>>& highlights = cell.highlights;
        unary_property<array<range>>& selections = cell.selections;
        binary_property<bool>& virtual_space = cell.virtual_space;
        binary_property<bool>& insert_mode = cell.insert_mode;
        binary_property<bool>& focused = cell.focused;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                XYWH r = coord.now.local();
                canvas.coord = r;
                frame .coord = r;
                refresh.go(true, time(1));
            }
            if (what == &text)
            {
                xmodel.text(text.now);
                html.now = xmodel.html();
                highlights = array<range>{};
                selections = array<range>{};
                refresh.go(true, time(1));
            }
            if (what == &html)
            {
                xmodel.html(html.now);
                text.now = xmodel.text();
                highlights = array<range>{};
                selections = array<range>{};
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
            if (what == &alignment
            or  what == &wordwrap
            or  what == &ellipsis
            or  what == &lpadding
            or  what == &rpadding
            or  what == &lwrap
            or  what == &rwrap)
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
                format.width  = coord.now.size.x; if (ellipsis.now)
                format.height = coord.now.size.y;

                model->set(style.now, format);
                cell.fill(model->lines);
            }
            if (what == &refresh
            or  what == &shift)
            {
                int H = coord.now.size.y;
                int h = cell.coord.now.size.y;

                cell.move_to(XY(0, H > h and
                    alignment.now.y == center ? H/2 - h/2 :
                    alignment.now.y == bottom ? H   - h   :
                    0) + shift.now);
            }
            if (what == &selections)
            {
                if (selections.now.size() == 1) {
                XYWH r = cell.carets(0).coord.now;
                r.x = 0; r.w = coord.now.w;
                current_line_frame.coord = r;
                current_line_frame.show(); } else
                current_line_frame.hide();
            }

            notify(what);
        }

        void on_focus (bool on) override { cell.on_focus(on); }

        auto selected () { return cell.selected(); }
    };
}
