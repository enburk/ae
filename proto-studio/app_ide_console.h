#pragma once
#include "abc.h"
using namespace pix;

struct Console:
widget<Console>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    gui::console editor, search, events, output;
    std::array<gui::console*, 4> consoles = {&editor , &search , &events , &output };
    std::array<std::string,   4> titles   = {"editor", "search", "events", "output"};

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

        editor.focusable = false;
    }

    void activate (gui::console * console)
    {
        for (int i=0; i<consoles.size(); i++) {
            buttons(i).on = console == consoles[i];
            consoles[i]->show(buttons(i).on.now);
        }
    }

    auto active () -> gui::console*
    {
        for (int i=0; i<consoles.size(); i++)
            if (buttons(i).on.now)
                return consoles[i];

        return nullptr;
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
            int h = gui::metrics::text::height*12/10;
            int w = 4*h;

            canvas .coord = xywh(0, 0, W, h);
            buttons.coord = xywh(0, 0, W, h);

            for (int i=0; i<consoles.size(); i++)
            {
                auto & console = *consoles[i];
                console.coord = xyxy(0, h, W, H);
                console.page.font = pix::font{"Consolas"};
                buttons(i).coord = xywh(W-i*w-w, 0, w, h);
            }
        }

        if (what == &buttons)
        for (int i=0; i<consoles.size(); i++)
        consoles[i]->show(buttons(i).on.now);

        for (auto c: consoles) if (what == &c->page.link)
        {
            str s = c->page.link;
            s.split_by("?", pressed_file, s);
            s.split_by(":", pressed_line, s);
            s.split_by(":", pressed_char, s);
            notify();
        }
    }
};
 