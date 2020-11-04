#pragma once
#include "gui_widget.h"
#include "gui_widget_canvas.h"
#include "sys_ui.h"
using namespace aux;
using namespace pix;

struct IDE : gui::widget<IDE>
{
    gui::canvas canvas;
    
    IDE()
    {
        canvas.color = RGBA::red;
    }

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            canvas.coord = coord.now.local();
        }
    }
};
sys::app<IDE> app("AE proto-studio");//, {0,0}, {100, 100});



