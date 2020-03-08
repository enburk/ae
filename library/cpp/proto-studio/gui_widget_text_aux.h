#pragma once
#include "../sys.h"
#include "gui_widgetarium.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    struct glyph:
    widget<glyph>
    {
        unary_property<sys::glyph> value;

        Opacity opacity () override { return semitransparent; }

        void on_change (void* what) override {
             if (what == &value) resize(XY(value.now.width,
                 value.now.ascent + value.now.descent));
        }
        void on_render (pix::frame<RGBA> frame, XY offset, uint8_t alpha) override {
             value.now.render(frame, offset, alpha, coord.now.x);
        }
    };

    struct token final : widgetarium<glyph>, sys::glyph_metrics
    {
        str text; sys::glyph_style_index style; void fill (
        str text, sys::glyph_style_index style)
        {
            if (this->text == text && this->style == style) return;
                this->text =  text;   this->style =  style;

            width = ascent = descent = advance = 0;

            glyphs = unicode::glyphs(text);
            for (int i=0; i<glyphs.size(); i++)
            {
                glyph & glyph = i < size() ? (*this)(i) : emplace_back();
                glyph.value = sys::glyph(glyphs[i], style);
                ascent  = max(ascent,  glyph.value.now.ascent);
                descent = max(descent, glyph.value.now.descent);
            }
            truncate(glyphs.size());
            for (auto & glyph : *this)
            {
                glyph.move_to(XY(width + advance, ascent - glyph.value.now.ascent));
                width  += advance + glyph.value.now.width;
                advance = glyph.value.now.advance;
            }
            resize(XY(width, ascent + descent));
        }

        array<str> glyphs;
    };

    ///////////////////////////////////////////////////////////////////////
                                const  int
    ///////////////////////////////////////////////////////////////////////
                                top    =-1,
    justify_left =-2, left =-1, center = 0, right = 1, justify_right = 2,
                                bottom = 1;
    ///////////////////////////////////////////////////////////////////////

    struct caret final : widget<caret>
    {
        canvas canvas;
        property<time> timer;
        binary_property<bool> insert_mode = true;
        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go (time::infinity,
                            time::infinity);

            if (what == &coord && coord.now.size != coord.was.size
            ||  what == &insert_mode) {
                XYWH r = coord.now.local();
                if (insert_mode.now) r.w = max (1, r.h/16);
                canvas.coord = r;
            }
            if (what == &timer) {
                int ms = time::now.ms % 1024;
                if (ms > 512) ms = 1024-ms-1;
                canvas.alpha = 192 * ms/512 + 64 - 1;
            }
        }
    };
} 