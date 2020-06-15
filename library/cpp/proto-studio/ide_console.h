#pragma once
#include "gui_widget_canvas.h"
#include "gui_widget_text_console.h"
using namespace pix;

struct Console : gui::widget<Console>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    gui::text::console editor;
    gui::text::console compiler;
    std::array<gui::text::console*, 2> consoles = {&editor, &compiler};
    static inline const str titles [] = {"Editor", "Compiler"};

    Console ()
    {
        for (int i=0; i<consoles.size(); i++)
            buttons(i).text.text = titles[i];

        buttons(0).on = true;
        for (int i=1; i<consoles.size(); i++)
            consoles[i]->hide();
    }

    void activate (gui::text::console * console)
    {
        for (auto & button : buttons) button.on = false;
        if (console == &editor  ) buttons(0).on = true;
        if (console == &compiler) buttons(1).on = true;

        for (int i=0; i<consoles.size(); i++)
            consoles[i]->show(buttons(i).on.now);
    }

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height * 12/7;
            int w = 5*h;

            canvas.coord = XYWH(0, 0, W, h);
            buttons.coord = XYWH(0, 0, W, h);

            for (int i=0; i<consoles.size(); i++)
            {
                buttons(i).coord = XYWH(W - i*w - w, 0, w, h);

                auto & console = *consoles[i];
                console.coord = XYXY(0, h, W, H);
                console.page.view.ground.color = gui::skins[skin.now].light.back_color;
                console.page.alignment = XY{gui::text::left, gui::text::top};
                console.page.style = sys::glyph_style{
                    sys::font{"Consolas", gui::metrics::text::height},
                    pix::black };
            }
        }
        if (what == &skin)
        {
            canvas.color = gui::skins[skin.now].light.back_color;
        }
    }

    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &buttons) {
            for (int i=0; i<consoles.size(); i++)
                consoles[i]->show(buttons(i).on.now);
        }
    }
};
 