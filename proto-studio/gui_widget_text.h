#pragma once
#include "gui_widgetarium.h"
#include "gui_widget_canvas.h"
namespace gui::text
{
    using pix::text::style;
    using pix::text::style_index;
    using pix::text::metrics;

    struct glyph:
    widget<glyph>
    {
        unary_property<pix::glyph> value;

        Opacity opacity () override { return semitransparent; }

        void on_change (void* what) override {
             if (what == &value) { resize(XY(value.now.width,
                 value.now.ascent + value.now.descent));
                 update();
             }
        }
        void on_render (sys::window& window, XYWH r, XY offset, uint8_t alpha) override {
             window.render(r, alpha, value.now, offset, coord.now.x);
        }
    };

    struct caret:
    widget<caret>
    {
        canvas image;
        property<time> timer;
        property<double> breadth = 0.15;
        binary_property<bool> insert_mode = true;

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go (time::infinity,
                          time::infinity);

            if (what == &timer)
            {
                const int MS = 1024;
                int ms = time::now.ms % MS;
                if (ms > MS/2) ms = MS-ms;
                image.alpha = aux::clamp<uint8_t>
                    (256 * ms/(MS/2) + 00 - 1);
            }
            if (what == &skin
            or  what == &insert_mode)
            {
                image.color = insert_mode.now ?
                skins[skin.now].dark.first:
                skins[skin.now].heavy.first;
            }
            if((what == &coord and coord.now.size != coord.was.size)
            or  what == &insert_mode
            or  what == &breadth)
            {
                XYWH r = coord.now.local();
                if (insert_mode.now) r.w = max (1, (int)(r.w*breadth.now));
                image.coord = r;
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
        bool operator == (place p) const { return line == p.line && (offset == p.offset); }
        bool operator != (place p) const { return line != p.line || (offset != p.offset); }
        bool operator <= (place p) const { return line <  p.line || (offset <= p.offset && line == p.line); }
        bool operator <  (place p) const { return line <  p.line || (offset <  p.offset && line == p.line); }
        bool operator >= (place p) const { return line >  p.line || (offset >= p.offset && line == p.line); }
        bool operator >  (place p) const { return line >  p.line || (offset >  p.offset && line == p.line); }
    };

    struct range
    {
        place from, upto;
        bool empty () const { return from == upto; }
        bool operator == (range r) const { return from == r.from && upto == r.upto; }
        bool operator != (range r) const { return from != r.from || upto != r.upto; }
    };
} 