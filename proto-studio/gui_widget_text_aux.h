#pragma once
#include "doc.h"
#include "gui_widgetarium.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    using pix::text::style;
    using pix::text::style_index;
    using pix::text::metrics;
    using pix::text::format;
    using doc::place;
    using doc::range;

    struct glyph:
    widget<glyph>
    {
        unary_property<pix::glyph> value;

        Opacity opacity () override { return semitransparent; }

        void on_change (void* what) override {
             if (what == &value) {
                 resize(XY(
                 value.now.width,
                 value.now.ascent +
                 value.now.descent));
                 update();
             }
        }
        void on_render (sys::window& window, XYWH r, XY offset, uint8_t alpha) override {
             window.render(r, alpha, value.now, offset, coord.now.x);
        }
    };

    struct token final : widgetarium<glyph>, metrics
    {
        array<str> glyphs;
        str text; str info; style_index style; void fill (
        str text, str info, style_index style)
        {
                this->info =  info;
            if (this->text == text && this->style == style) return;
                this->text =  text;   this->style =  style;

            width = ascent = descent = advance = 0;

            XY shift = style.style().shift;

            glyphs = unicode::glyphs(text); reserve(glyphs.size());

            for (int i=0; i<glyphs.size(); i++)
            {
                glyph & glyph = (*this)(i);
                glyph.value = sys::glyph(glyphs[i], style);
                ascent  = max(ascent,  shift.y + glyph.value.now.ascent);
                descent = max(descent, shift.y + glyph.value.now.descent);
            }

            truncate(glyphs.size());

            for (auto & glyph : *this)
            {
                glyph.move_to(shift + XY(width + advance, ascent - glyph.value.now.ascent));
                width  += shift.x + glyph.value.now.width + advance;
                advance = shift.x + glyph.value.now.advance;
            }

            resize(XY(width, ascent + descent));
        }
    };

    struct caret:
    widget<caret>
    {
        canvas image;
        property<time> timer;
        property<double> breadth = 0.15;
        binary_property<bool> insert_mode = true;
        static inline time time_moved;

        caret () { image.color = skins[skin].touched.first; }

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go (time::infinity,
                          time::infinity);

            if (what == &timer)
            {
                if (time::now - time_moved < 200ms) {
                    image.alpha = 255;
                    return;
                }
                const int MS = 1024;
                int ms = time::now.ms % MS;
                if (ms > MS/2) ms = MS-ms;
                auto a = data::clamp<uint8_t>
                (256 * ms/(MS/2) + 00 - 1);
                if (true) a = a > 127 ? 255 : 0;
                image.alpha = a;
                return;
            }
            if (what == &skin
            or  what == &insert_mode)
            {
                image.color = insert_mode.now ?
                skins[skin].dark.first:
                skins[skin].heavy.first;
            }
            if (what == &coord
            or  what == &insert_mode
            or  what == &breadth)
            {
                time_moved = time::now;
                XYWH r = coord.now.local();
                if (insert_mode.now) r.w = max (1, (int)(r.w*breadth.now));
                image.coord = r;
            }
        }
    };
} 