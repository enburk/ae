#pragma once
#include "gui_widget_text_column.h"
#include "gui_widget_text_model.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view:
    widget<view>
    {
        canvas ground;
        widgetarium<canvas> highlight_bars;
        widgetarium<canvas> selection_bars;
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

        unary_property<array<range>> highlights;
        unary_property<array<range>> selections;

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
            column.fill(model.lines);
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

            highlight(highlights.now, highlight_bars, skins[skin.now].highlight);
            highlight(selections.now, selection_bars, skins[skin.now].selection);
        }

        void highlight (array<range> ranges, widgetarium<canvas> & bars, RGBA color)
        {
            int n = 0;

            for (auto [from, upto] : ranges)
            {
                if (from > upto) std::swap (from, upto);

                for (; from.line <= upto.line; from.line++, from.offset = 0)
                {
                    if (from.line >= column.size()) break;
                    int from_offset = from.offset;
                    int upto_offset = from.line == upto.line ?
                        upto.offset : column(from.line).length();
                    if (from_offset >= upto_offset) continue;

                    int x = column.coord.now.x + column(from.line).coord.now.x;
                    int y = column.coord.now.y + column(from.line).coord.now.y;
                    int offset = 0;

                    for (auto & token : column(from.line))
                    {
                        if (upto_offset <= offset) break;
                        if (from_offset <= offset + token.size() - 1)
                        {
                            int from_glyph = max(from_offset - offset, 0);
                            int upto_glyph = min(upto_offset - offset, token.size());

                            auto & g1 = token(from_glyph);
                            auto & g2 = token(upto_glyph-1);
                            auto & bar = bars(n++);
                            bar.color = color;
                            bar.coord = XYXY (
                                x + token.coord.now.x + g1.coord.now.x,
                                y + token.coord.now.y + g1.coord.now.y,
                                x + token.coord.now.x + g2.coord.now.x + g2.coord.now.w,
                                y + token.coord.now.y + g2.coord.now.y + g2.coord.now.h
                            );
                        }
                        offset += token.size();
                    }
                }
            }

            bars.truncate(n);
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                ground.coord = coord.now.local();
                highlight_bars.coord = coord.now.local();
                selection_bars.coord = coord.now.local();
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
            if (what == &highlights)
            {
                highlight(highlights.now, highlight_bars,
                    skins[skin.now].highlight);
            }
            if (what == &selections)
            {
                highlight(selections.now, selection_bars,
                    skins[skin.now].selection);
            }
        }
    };
} 