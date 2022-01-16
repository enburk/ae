#pragma once
#include "gui_widget_canvas.h"
#include "gui_widget_console.h"
using namespace pix;

struct Console : gui::widget<Console>
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
        for (int i=0; i<consoles.size(); i++) {
            if (buttons(i).on.now)
                return consoles[i];
        }
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

        if (what == &buttons)
            for (int i=0; i<consoles.size(); i++)
                consoles[i]->show(buttons(i).on.now);
    }

    void on_mouse_press (XY p, char button, bool down) override
    {
        if (button != 'L' or not down) return;

        pressed_file = "";
        pressed_line = "";
        pressed_char = "";

        for (int i=0; i<consoles.size(); i++)
        {
            if (not buttons(i).on.now) continue;

            auto & console = *consoles[i];
            std::lock_guard guard{console.mutex};
            auto & column = console.page.view.column;

            p -= console.coord.now.origin;

            if (not column.coord.now.includes(p)) return;
            auto cp = p - column.coord.now.origin;
            for (int ln=0; ln<column.size(); ln++)
            {
                if (not column(ln).coord.now.includes(cp)) continue;

                while (ln >= 0)
                {
                    auto & tokens = column(ln).tokens;
                    str line; for (auto & t : tokens) line += t.text;
                    line.replace_all("\n", "");

                    if (not line.starts_with("(")) { ln--; continue; }

                    line.split_by("(", pressed_line, line);
                    line.split_by(":", pressed_line, line);
                    line.split_by(")", pressed_char, line);

                    break;
                }

                ln--; if (pressed_line == "") return;

                while (ln >= 0)
                {
                    auto & tokens = column(ln).tokens;
                    str line; for (auto & t : tokens) line += t.text;
                    line.replace_all("\n", "");

                    if (line.starts_with("(")) { ln--; continue; }

                    if (line.ends_with(".ae")
                    or  line.ends_with(".ae!")
                    or  line.ends_with(".ae!!"))
                        pressed_file = line;

                    break;
                }

                break;
            }

            break;
        }

        if (pressed_line != "") notify();
    }
};
 