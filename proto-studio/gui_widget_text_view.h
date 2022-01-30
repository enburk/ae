#pragma once
#include "doc_html_model.h"
#include "gui_widget_text_cell.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view:
    widget<view>
    {
        using color_bars = widgetarium<canvas>;

        canvas canvas;
        color_bars highlight_bars;
        color_bars selection_bars; cell cell;
        widgetarium<caret> carets;
        frame current_line_frame;
        frame frame;

        column& lines = cell.column;
        unary_property<str>& text = cell.text;
        unary_property<str>& html = cell.html;
        property<RGBA>& color = cell.color;
        binary_property<font>& font = cell.font;
        binary_property<style>& style = cell.style;
        binary_property<XY>& alignment = cell.alignment;
        binary_property<XY>& shift = cell.shift;
        binary_property<padding>& lpadding = cell.lpadding;
        binary_property<padding>& rpadding = cell.rpadding;
        binary_property<bool>& wordwrap = cell.wordwrap;
        binary_property<bool>& ellipsis = cell.ellipsis;
        property<bool>& refresh = cell.refresh;

        unary_property<array<range>> highlights;
        unary_property<array<range>> selections;

        binary_property<bool> insert_mode = true;
        binary_property<bool> virtual_space = false;
        binary_property<bool> focused = false;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                XYWH r = coord.now.local();
                carets.coord = r;
                canvas.coord = r;
                highlight_bars.coord = r;
                selection_bars.coord = r;
                frame.coord = r;
                cell.coord = r;
            }
            if (what == &text
            or  what == &html)
            {
                highlights = array<range>{};
                selections = array<range>{};
            }
            if (what == &shift)
            {
                lines.move_to(shift);
                align();
            }
            if (what == &refresh)
            {
                align();
            }
            if (what == &highlights)
            {
                highlight(
                    highlights.now,
                    highlight_bars, skins[skin.now].
                    highlight.first);
            }
            if (what == &selections)
            {
                highlight(
                    selections.now,
                    selection_bars, skins[skin.now].
                    selection.first);

                refresh_carets ();
            }
            if (what == &focused ||
                what == &insert_mode)
            {
                refresh_carets();
            }

            notify(what);
        }

        void align ()
        {
            highlight(highlights.now, highlight_bars, skins[skin.now].highlight.first);
            highlight(selections.now, selection_bars, skins[skin.now].selection.first);
            refresh_carets ();
        }

        void refresh_carets ()
        {
            int n = 0;

            for (auto range : selections.now)
                carets(n++).coord = cell.bar(
                    range.upto, virtual_space.now);

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
                for (XYWH r : cell.bars(range))
                {
                    auto & bar = bars(n++);
                    bar.color = color;
                    bar.coord = r;
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
                    if (from.line >= cell.column.size()) break;
                    int from_offset = from.offset;
                    int upto_offset = from.line == upto.line ?
                        upto.offset : cell.column(from.line).length;
                    if (from_offset >= upto_offset) continue;

                    int offset = 0;

                    for (auto & token : cell.column(from.line))
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

        void on_focus (bool on) override { focused = on; }
    };
} 