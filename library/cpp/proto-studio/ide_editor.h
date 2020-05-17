#pragma once
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget_button.h"
#include "gui_widget_scroller.h"
#include "gui_widget_text_editor.h"
using namespace pix;

struct Editor : gui::widget<Editor>
{
    str filename;

    gui::text::editor editor;

    void load (std::filesystem::path path)
    {
        std::ifstream stream(path); str text = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};

        if (text.starts_with("\xEF" "\xBB" "\xBF"))
            text.upto(3).erase(); // UTF-8 BOM

        editor.set(text, "text");
    }

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int h = gui::metrics::text::height;

            editor.coord = coord.now.local();
            editor.virtual_space = true;
            editor.page.view.word_wrap = false;
            editor.page.view.ground.color = pix::white;
            editor.page.style = sys::glyph_style{
                sys::font{"Consolas", h*110/100},
                pix::black };
        }
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }

    void on_notify (gui::base::widget* w) override
    {
    }
    void on_notify (gui::base::widget* w, int n) override
    {
    }
};
