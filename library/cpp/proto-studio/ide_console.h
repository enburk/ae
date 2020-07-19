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
    gui::button copy;
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

        copy.text.text = "copy";
    }

    void activate (gui::text::console * console)
    {
        for (auto & button : buttons) button.on = false;
        if (console == &editor  ) buttons(0).on = true;
        if (console == &compiler) buttons(1).on = true;

        for (int i=0; i<consoles.size(); i++)
            consoles[i]->show(buttons(i).on.now);

        refresh();
    }

    void refresh ()
    {
        for (int i=0; i<consoles.size(); i++)
            if (buttons(i).on.now) copy.show(
                consoles[i]->page.view.selected() != "");
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
            copy.coord = XYWH(0, 0, w/2, h);

            for (int i=0; i<consoles.size(); i++)
            {
                buttons(i).coord = XYWH(W - i*w - w, 0, w, h);

                auto & console = *consoles[i];
                console.coord = XYXY(0, h, W, H);
                console.page.view.ground.color = gui::skins[skin.now].ultralight.back_color;
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

    void on_notify (gui::base::widget* w) override
    {
        if (w == &copy) {
            for (int i=0; i<consoles.size(); i++)
                if (buttons(i).on.now)
                    sys::clipboard::set(
                        consoles[i]->page.view.selected());
        }
    }

    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &buttons) {
            for (int i=0; i<consoles.size(); i++)
                consoles[i]->show(buttons(i).on.now);
        }
        refresh();
    }

    void on_mouse_press (XY p, char button, bool down) override
    {
        refresh();

        if (button != 'L') return;
        if (not down) return;

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
                    auto & tokens = column(ln).data_copy.tokens;
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
                    auto & tokens = column(ln).data_copy.tokens;
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
 