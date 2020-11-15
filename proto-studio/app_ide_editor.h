#pragma once
#include "gui_widget_canvas.h"
using namespace pix;

struct Editor : gui::widget<Editor>
{
    gui::canvas canvas;

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            canvas.color = gui::skins[skin].light.first;
        }
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*12/7;
            //int w = 5*h;

            canvas.coord = XYWH(0, 0, W, h);
        }
    }
};
