#pragma once
#include "../sys.h"
#include "gui_widgetarium.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    using sys::glyph_style;
    using sys::glyph_style_index;
    using sys::glyph_metrics;

    struct glyph:
    widget<glyph>
    {
        unary_property<sys::glyph> value;

        Opacity opacity () override { return semitransparent; }

        void on_change (void* what) override {
             if (what == &value) { resize(XY(value.now.width,
                 value.now.ascent + value.now.descent));
                 update();
             }
        }
        void on_render (pix::frame<RGBA> frame, XY offset, uint8_t alpha) override {
             value.now.render(frame, offset, alpha, coord.now.x);
        }
    };

    ///////////////////////////////////////////////////////////////////////

    struct caret final : widget<caret>
    {
        canvas canvas;
        property<time> timer;
        binary_property<bool> insert_mode = true;

        caret () { canvas.color = skins[skin.now].touched.back_color; }

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go (time::infinity,
                          time::infinity);

            if (what == &insert_mode)
            {
                canvas.color = insert_mode.now ?
                    skins[skin.now].black:
                    skins[skin.now].heavy.back_color;
            }
            if (what == &coord && coord.now.size != coord.was.size
            ||  what == &insert_mode)
            {
                XYWH r = coord.now.local();
                if (insert_mode.now) r.w = max (1, r.h/10);
                canvas.coord = r;
            }
            if (what == &timer)
            {
                const int MS = 1024;
                int ms = time::now.ms % MS;
                if (ms > MS/2) ms = MS-ms;
                canvas.alpha = aux::clamp<uint8_t>
                    (256 * ms/(MS/2) + 00 - 1);
            }
        }
    };

    ///////////////////////////////////////////////////////////////////////
                                const  int
    ///////////////////////////////////////////////////////////////////////
                                top    =-1,
    justify_left =-2, left =-1, center = 0, right = 1, justify_right = 2,
                                bottom = 1;
    ///////////////////////////////////////////////////////////////////////

    struct format
    {
        int width  = max<int>();
        int height = max<int>();

        XY alignment = XY{center, center};

        bool word_wrap = true;
        bool ellipsis = false;

        XY margin_left;
        XY margin_right;

        bool operator != (const format & f) const { return !(*this == f); }
        bool operator == (const format & f) const { return true
            && width        == f.width       
            && height       == f.height      
            && alignment    == f.alignment   
            && word_wrap    == f.word_wrap   
            && ellipsis     == f.ellipsis    
            && margin_left  == f.margin_left 
            && margin_right == f.margin_right;
        }
    };

    ///////////////////////////////////////////////////////////////////////

    struct place
    {
        int line = 0, offset = 0; // bool operator <=> (place p) const = default;
        bool operator == (place p) const { return line == p.line && offset == p.offset; }
        bool operator != (place p) const { return line != p.line || offset != p.offset; }
        bool operator <= (place p) const { return line <  p.line || offset <= p.offset && line == p.line; }
        bool operator <  (place p) const { return line <  p.line || offset <  p.offset && line == p.line; }
        bool operator >= (place p) const { return line >  p.line || offset >= p.offset && line == p.line; }
        bool operator >  (place p) const { return line >  p.line || offset >  p.offset && line == p.line; }
    };

    struct range
    {
        place from, upto;
        bool empty () const { return from == upto; }
        bool operator == (range r) const { return from == r.from && upto == r.upto; }
        bool operator != (range r) const { return from != r.from || upto != r.upto; }
    };
} 