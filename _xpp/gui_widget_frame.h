#pragma once
#include "gui_widget.h"
namespace gui
{
    struct frame : widget
    {
        property<RGBA>   color;
        property<int>    thickness;
        property<int8_t> embossed;

        frame (widget* parent) : widget (parent) { thickness.go(1); }

        void on_tick () override
        {
            color.tick(); thickness.tick(); embossed.tick();

            if (color    .now == color    .was &&
                thickness.now == thickness.was &&
                embossed .now == embossed .was) return;

            int w = coord.now.w;
            int h = coord.now.h;
            int d = std::max(thickness.was, thickness.now);
            update (XYWH(0, 0,   w, d));
            update (XYWH(0, h-d, w, d));
            update (XYWH(0,   d, d, h-d-d));
            update (XYWH(w-d, d, d, h-d-d));
        }

        void on_render () override
        {
            int w = coord.now.w;
            int h = coord.now.h;
            int d = thickness.now;
            int8_t e = embossed.now;
            RGBA c1 = color.now;
            RGBA c2 = color.now;
            if (e != 0) {
                 c1.blend(pix::white, abs(e));
                 c2.blend(pix::black, abs(e));
                 if (e < 0) std::swap (c1,c2);
            }
            fill (XYWH(0, 0,   w, d    ), c1);
            fill (XYWH(0, h-d, w, d    ), c2);
            fill (XYWH(0,   d, d, h-d-d), c1);
            fill (XYWH(w-d, d, d, h-d-d), c2);
        }
    };
}
 