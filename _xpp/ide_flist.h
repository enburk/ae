#pragma once
#include "gui_widget_frame.h"
using namespace pix;

struct Flist : gui::widget
{
    gui::frame frame1 = gui::frame(this);
    gui::frame frame2 = gui::frame(this);

    Flist(gui::widget* parent) : gui::widget(parent)
    {
        frame1. color.go(pix::gray);
        frame2. color.go(pix::black);
        widget::color.go(pix::silver);
    }

    void on_tick () override
    {
        if (coord.was.size() != coord.now.size())
        {
            auto r = coord.now - coord.now.origin();
            frame1.move_to(r); r.deflate(frame1.thickness.now);
            frame2.move_to(r); r.deflate(frame2.thickness.now);
        }
    }
}; 