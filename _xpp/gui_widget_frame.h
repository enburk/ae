#pragma once
#include "gui_widget.h"
namespace gui
{
    struct frame : widget
    {
        property<RGBA> color;
        property<int>  thickness;

        frame (widget* parent) : widget (parent) { thickness.go(5); }

        void on_tick () override
        {
            color.tick(); thickness.tick();

            if (color.now == color.was && thickness.now == thickness.was) return;

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
            fill (XYWH(0, 0,   w, d), color.now);
            fill (XYWH(0, h-d, w, d), color.now);
            fill (XYWH(0,   d, d, h-d-d), color.now);
            fill (XYWH(w-d, d, d, h-d-d), color.now);
        }
    };
}
 