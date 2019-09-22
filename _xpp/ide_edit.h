#pragma once
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget_canvas.h"
using namespace pix;

struct Editor : gui::widget<Editor>
{
    str filename;

    gui::frame frame1;
    gui::frame frame2;

    Editor()
    {
        frame1. color = pix::gray;
        frame2. color = pix::black;
        widget::color = pix::white;
    }

    void on_change () override
    {
        if (coord.was.size() != coord.now.size())
        {
            auto r = coord.now - coord.now.origin();
            frame1.move_to(r); r.deflate(frame1.thickness.now);
            frame2.move_to(r); r.deflate(frame2.thickness.now);
        }
    }
};
