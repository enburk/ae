#pragma once
#include "gui_widget_canvas.h"
#include "gui_widget_console.h"
using namespace pix;

struct Console : gui::widget<Console>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    gui::console editor, search, events;
    std::array<gui::console*,3> consoles = {&editor , &search , &events };
    std::array<std::string,  3> titles   = {"editor", "search", "events"};

    str pressed_file;
    str pressed_line;
    str pressed_char;

    Console ()
    {
        for (int i=0; i<consoles.size(); i++)
            buttons(i).text.text = titles[i];

        buttons(0).on = true;
        for (int i=1; i<consoles.size(); i++)
            consoles[i]->hide();
    }

    void activate (gui::console * console)
    {
        for (int i=0; i<consoles.size(); i++) {
            buttons(i).on = console == consoles[i];
            consoles[i]->show(buttons(i).on.now);
        }
    }

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
            int w = 4*h;

            canvas .coord = XYWH(0, 0, W, h);
            buttons.coord = XYWH(0, 0, W, h);

            for (int i=0; i<consoles.size(); i++)
            {
                buttons(i).coord = XYWH(W - i*w - w, 0, w, h);

                auto & console = *consoles[i];
                console.coord = XYXY(0, h, W, H);
                console.page.view.canvas.color = gui::skins[skin].ultralight.first;
                console.page.alignment = XY{pix::left, pix::top};
                console.page.style = pix::text::style{
                    sys::font{"Consolas", gui::metrics::text::height},
                    RGBA::black };
            }
        }
    }

    void on_notify (void* what) override
    {
        if (what == &buttons)
            for (int i=0; i<consoles.size(); i++)
                consoles[i]->show(buttons(i).on.now);
    }
};
 