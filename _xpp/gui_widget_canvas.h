#pragma once
#include "pix.h"
#include "gui_widget.h"
namespace gui
{
    struct canvas : widget<canvas>
    {
        canvas (base::widget* parent = nullptr) : widget (parent) {}
    };

    struct frame : widget<frame>
    {
        frame (base::widget* parent = nullptr) : widget (parent) {}

        property<RGBA> color;
        property<int>  thickness = 1;

        canvas l, t, r, b;

        void on_change () override
        {
            int w = coord.now.w;
            int h = coord.now.h;
            int d = thickness.now;
            t.move_to (XYWH(0, 0,   w, d));
            b.move_to (XYWH(0, h-d, w, d));
            l.move_to (XYWH(0,   d, d, h-d-d));
            r.move_to (XYWH(w-d, d, d, h-d-d));
            t.color = color.now;
            b.color = color.now;
            l.color = color.now;
            r.color = color.now;
        }
    };
}
