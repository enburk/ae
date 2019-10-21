#pragma once
#include "gui_widget_canvas.h"
using namespace pix;

struct Flist : gui::widget<Flist>
{
    gui::canvas canvas;
    gui::frame  frame1;
    gui::frame  frame2;

    Flist()
    {
        frame1.color = pix::gray;
        frame2.color = pix::black;
        canvas.color = pix::silver;
    }

    void on_change () override
    {
        if (coord.was.size != coord.now.size)
        {
            auto r = coord.now - coord.now.origin;
            frame1.move_to(r); r.deflate(frame1.thickness.now);
            frame2.move_to(r); r.deflate(frame2.thickness.now);
            canvas.move_to(r);
        }
    }
}; 