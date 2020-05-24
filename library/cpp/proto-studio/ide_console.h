#pragma once
#include "gui_widget_canvas.h"
#include "gui_widget_text_console.h"
using namespace pix;

struct Console : gui::widget<Console>
{
    gui::text::console console;

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int h = gui::metrics::text::height;

            console.coord = coord.now.local();
            console.page.view.ground.color = gui::skins[skin.now].light.back_color;
            console.page.alignment = XY{gui::text::left, gui::text::top};
            console.page.style = sys::glyph_style{
                sys::font{"Consolas", h*110/100},
                pix::black };
        }
    }
};
 