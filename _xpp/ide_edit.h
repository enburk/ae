#pragma once
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget_frame.h"
using namespace pix;

struct Editor : gui::widget
{
    str filename;

    gui::frame frame1 = gui::frame(this);
    gui::frame frame2 = gui::frame(this);

    Editor(gui::widget* parent) : gui::widget(parent)
    {
        frame1. color.go(pix::gray);
        frame2. color.go(pix::black);
        widget::color.go(pix::white);
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
