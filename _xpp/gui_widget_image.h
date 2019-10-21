#pragma once
#include "gui_widget.h"
namespace gui
{
    struct image : widget<image>
    {
        pix::Image<RGBA> image;

        void on_render (Frame<RGBA> frame, XY offset, uint8_t alpha) override
        {
            frame.blend_from(image.frame(XYWH(offset.x, offset.y, frame.size.x, frame.size.y)), alpha);
        }

        void on_change () override
        {
        }
    };
}
