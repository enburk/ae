#pragma once
#include "gui_widget_canvas.h"
using namespace pix;

struct Console : gui::widget<Console>
{
    gui::canvas canvas;
    gui::frame  frame1;
    gui::frame  frame2;

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            frame1.color = gui::schemas[""].light.back_color;
            frame2.color = gui::schemas[""].heavy.back_color;
            canvas.color = gui::schemas[""].light.back_color;

            auto r = coord.now.local();
            frame1.move_to(r); r.deflate(frame1.thickness.now);
            frame2.move_to(r); r.deflate(frame2.thickness.now);
            canvas.move_to(r);
        }
    }
};
 