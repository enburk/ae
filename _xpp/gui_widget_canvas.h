#pragma once
#include "gui_widget.h"
namespace gui
{
    struct canvas final : widget<canvas>
    {
        property<RGBA> color;

        Opacity opacity () override { return
            color.now.a == 255 ? opaque :
            color.now.a == 0 ? transparent :
                             semitransparent; }

        void on_render (pix::frame<RGBA> frame, XY offset, uint8_t alpha) override
        {
            frame.blend(color.now, alpha);
        }

        void on_change (void* what) override
        {
            if (what == &color)
                if (color.was.a != 0 ||
                    color.now.a != 0 ) update();
        }
    };

    struct frame final : widget<frame>
    {
        property<RGBA> color;
        property<int> thickness = 1;

        canvas l, t, r, b;

        void on_change () override
        {
            int w = coord.now.w;
            int h = coord.now.h;
            int d = thickness.now;
            t.coord = XYWH(0, 0,   w, d);
            b.coord = XYWH(0, h-d, w, d);
            l.coord = XYWH(0,   d, d, h-d-d);
            r.coord = XYWH(w-d, d, d, h-d-d);
            t.color = color.now;
            b.color = color.now;
            l.color = color.now;
            r.color = color.now;
        }
    };
}
