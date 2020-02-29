#pragma once
#include "gui_widget_canvas.h"
using namespace pix;

struct Console : gui::widget<Console>
{
    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
        }
    }
};
 