#pragma once
#include "doc_html_model.h"
#include "gui_widget_text_column.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view:
    widget<view>
    {
        canvas canvas; frame frame;
        widgetarium<gui::canvas> highlight_bars;
        widgetarium<gui::canvas> selection_bars;
        column column;

        unary_property<str> text;
        unary_property<str> html;

        property<RGBA> color;
        binary_property<font> font;
        binary_property<style> style;
        binary_property<bool> word_wrap = true;
        binary_property<bool> ellipsis = false;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> margin_right;
        binary_property<XY> margin_left;
        binary_property<XY> padding;
        binary_property<XY> shift;

        unary_property<array<range>> highlights;
        unary_property<array<range>> selections;

        widgetarium<caret> carets;
        binary_property<bool> insert_mode = true;
        binary_property<bool> virtual_space = false;
        binary_property<bool> focused = false;
        gui::frame current_line_frame;

        doc::html::model _model;
        doc::view::model* model = &_model;

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

            model->set(style.now, format);
            column.fill(model->lines);
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

            highlight(highlights.now, highlight_bars, skins[skin.now].highlight.first);
            highlight(selections.now, selection_bars, skins[skin.now].selection.first);

            refresh_carets ();
        }

        void refresh_carets ()
        {
            int n = 0;

            for (auto range : selections.now)
                carets(n++).coord = column.bar(range.upto, virtual_space.now)
                    + column.coord.now.origin;

            carets.truncate(n);

            for (auto & caret : carets) {
                caret.insert_mode = insert_mode.now;
                caret.show(focused.now);
            }

            if (n == 1) {
                XYWH r = carets(0).coord.now;
                r.x = 0; r.w = coord.now.w;
                current_line_frame.coord = r;
                current_line_frame.show(); } else
                current_line_frame.hide();
        }

        void highlight (array<range> ranges, widgetarium<gui::canvas> & bars, RGBA color)
        {
            int n = 0;

            for (auto range : ranges)
            {
                for (XYWH r : column.bars(range))
                {
                    auto & bar = bars(n++);
                    bar.color = color;
                    bar.coord = r + column.coord.now.origin;
                }
            }

            bars.truncate(n);
        }

        str selected () const
        {
            str s;

            for (auto [from, upto] : selections.now)
            {
                if (from > upto) std::swap (from, upto);

                for (; from.line <= upto.line; from.line++, from.offset = 0)
                {
                    if (from.line >= column.size()) break;
                    int from_offset = from.offset;
                    int upto_offset = from.line == upto.line ?
                        upto.offset : column(from.line).length();
                    if (from_offset >= upto_offset) continue;

                    int offset = 0;

                    for (auto & token : column(from.line))
                    {
                        if (upto_offset <= offset) break;
                        if (from_offset <= offset + token.size() - 1)
                        {
                            int from_glyph = max(from_offset - offset, 0);
                            int upto_glyph = min(upto_offset - offset, token.size());

                            for (auto& g: token.glyphs
                                .from(from_glyph)
                                .upto(upto_glyph))
                                s += g.text;
                        }
                        offset += token.size();
                    }
                }
            }

            return s;
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                carets.coord = coord.now.local();
                canvas.coord = coord.now.local();
                frame .coord = coord.now.local();
                highlight_bars.coord = coord.now.local();
                selection_bars.coord = coord.now.local();
                refresh();
            }
            if (what == &text)
            {
                highlights = array<range>{};
                selections = array<range>{};
                _model.text(text.now);
                html.now = _model.html();
                refresh();
            }
            if (what == &html)
            {
                highlights = array<range>{};
                selections = array<range>{};
                _model.html(html.now);
                text.now = _model.text();
                refresh();
            }
            if (what == &skin)
            {
                style = pix::text::style
                {
                    skins[skin.now].font,
                    skins[skin.now].normal.second
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
            ||  what == &padding
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
            if (what == &highlights)
            {
                highlight(highlights.now, highlight_bars,
                    skins[skin.now].highlight.first);
            }
            if (what == &selections)
            {
                highlight(selections.now, selection_bars,
                    skins[skin.now].selection.first);

                refresh_carets ();
            }
            if (what == &focused || what == &insert_mode)
            {
                refresh_carets();
            }
        }

        void on_focus (bool on) override { focused = on; }
    };
} 