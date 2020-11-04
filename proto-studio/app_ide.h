#pragma once
#include "gui_widget.h"
#include "gui_widget_canvas.h"
#include "sys_aux.h"
#include "sys_ui.h"
using namespace aux;
using namespace pix;

struct IDE : gui::widget<IDE>
{
    gui::canvas canvas;

    gui::splitter splitter_editor_l;
    gui::splitter splitter_editor_r;
    
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

    void on_notify (void* what) override
    {
        if (what == &splitter_editor_l) {
            sys::settings::save(
                "splitter.editor.l.permyriad",
                 splitter_editor_l.middle
                * 10'000 / coord.now.w);
            on_change(&coord);
        }
        if (what == &splitter_editor_r) {
            sys::settings::save(
                "splitter.editor.r.permyriad",
                 splitter_editor_r.middle
                * 10'000 / coord.now.w);
            on_change(&coord);
        }
    }
};
sys::app<IDE> app("AE proto-studio");//, {0,0}, {100, 100});



