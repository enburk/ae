#pragma once
#include "gui_widget.h"
namespace gui
{
    struct image final : widget<image>
    {
        unary_property<pix::Frame<RGBA>> source;

        void on_render (Frame<RGBA> frame, XY offset, uint8_t alpha) override
        {
            frame.blend_from(
                source.now.frame(
                    XYWH(offset.x, offset.y,
                        frame.size.x, frame.size.y)),
                            alpha);
        }

        void on_change (void* what) override
        {
            if (what == &source) update();
        }
    };
}
