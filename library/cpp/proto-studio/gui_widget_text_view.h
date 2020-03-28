#pragma once
#include "gui_widget_text_page.h"
#include "gui_widget_text_model.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct view final : widget<view>
    {
        canvas canvas; page page;
        unary_property<str> text;
        unary_property<str> html;

        property<RGBA> color;
        binary_property<sys::font> font;
        binary_property<glyph_style> style;
        binary_property<bool> word_wrap = true;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> shift;

        htmlmodel model; array<doc::entity> entities;

        view () { on_change(&skin); }

        void refresh ()
        {
            format format;
            format.width = coord.now.size.x;
            format.alignment = alignment.now;
            format.word_wrap = word_wrap.now;

            model = htmlmodel(entities, glyph_style_index(style.now), format);
            page.fill (model.sections);
            page.move_to(XY(
                shift.now.x,
                shift.now.y + (
                alignment.now.y == center ? coord.now.size.y/2 - page.coord.now.size.y/2 :
                alignment.now.y == bottom ? coord.now.size.y   - page.coord.now.size.y   :
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
            ||  what == &alignment)
            {
                refresh();
            }
            if (what == &shift)
            {
                page.move_to(XY(
                    shift.now.x,
                    shift.now.y + (
                    alignment.now.y == center ? coord.now.size.y/2 - page.coord.now.size.y/2 :
                    alignment.now.y == bottom ? coord.now.size.y   - page.coord.now.size.y   :
                    0)));
            }
        }

        //struct Place { int line = 0, row = 0, col = 0; };
        //
        //Place place (int charnumber)
        //{
        //    for (auto & line : page) {
        //        if (line.size() == 0) continue;
        //        if (line.back().doc_token.offset +
        //            line.back().doc_token.text.size() <= charnumber) continue;
        //
        //        for (auto & token : line) {
        //            if (token.doc_token.offset +
        //                token.doc_token.text.size() <= charnumber) continue;
        //
        //
        //        int token_number = 0;
        //        for (auto & row : line.rows) {
        //            if (row.offsets.size() == 0) continue;
        //            token_number += row.offsets.size();
        //            if (line(token_number-1).doc_token.offset +
        //                line(token_number-1).doc_token.text.size() <= charnumber) continue;
        //        }
        //
        //    }
        //    return Place{page.size(), 0, 0};
        //}

        XYWH position (int n)
        {
            //for (auto & line : page) {
            //    if (line.size() == 0) continue;
            //    if (line.back().doc_token.offset +
            //        line.back().doc_token.text.size() <= n) continue;
            //
            //    for (auto & token : line) {
            //        if (token.doc_token.offset +
            //            token.doc_token.text.size() <= n) continue;
            //        return
            //            token.glyphs(n - token.doc_token.offset).coord.now + 
            //            token.coord.now.origin +
            //            line .coord.now.origin +
            //            page .coord.now.origin;
            //    }
            //}
            return XYWH();
        }
    };
} 