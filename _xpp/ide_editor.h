#pragma once
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget_text_editor.h"
#include "gui_widget_button.h"
using namespace pix;

struct Editor : gui::widget<Editor>
{
    str filename;

    gui::canvas canvas;
    gui::frame  frame1;
    gui::frame  frame2;

    gui::text::editor editor;

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            frame1.color = gui::schemas[""].light.back_color;
            frame2.color = gui::schemas[""].heavy.back_color;
            canvas.color = gui::schemas[""].light.back_color;

            editor.background.color = pix::white;
            editor.view.style = sys::glyph_style{
                sys::font{"Consolas", gui::metrics::text::height*110/100},
                pix::black };

            auto r = coord.now.local();
            frame1.coord = r; r.deflate(frame1.thickness.now);
            frame2.coord = r; r.deflate(frame2.thickness.now);
            canvas.coord = r; r.deflate(frame2.thickness.now);
            editor.coord = r;
        }
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
};
