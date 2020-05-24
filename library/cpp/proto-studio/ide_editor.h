#pragma once
#include "doc.h"
#include "doc_text_lexica.h"
#include "gui_widget_button.h"
#include "gui_widget_scroller.h"
#include "gui_widget_text_editor.h"
using namespace pix;

struct Editor : gui::widget<Editor>
{
    str filename;

    gui::text::view   lineup;
    gui::text::editor editor;

    Editor ()
    {
    }

    void load (std::filesystem::path path)
    {
        std::ifstream stream(path); str text = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};

        if (text.starts_with("\xEF" "\xBB" "\xBF"))
            text.upto(3).erase(); // UTF-8 BOM

        str ext = path.extension().string();

        if (ext == ".ae!" || ext == ".ae")
            ext =  "ae";
        else
        if (ext == ".cpp" || ext == ".hpp"
        ||  ext == ".cxx" || ext == ".hxx"
        ||  ext == ".c++" || ext == ".h++"
        ||  ext == ".h")
            ext =  "cpp";

        editor.set(text, ext);
    }

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height;
            int d = 3*h;

            lineup.coord = XYXY(0, 0, d, H);
            editor.coord = XYXY(d, 0, W, H);
        }
        if (what == &skin)
        {
            int h = gui::metrics::text::height;

            lineup.ground.color = gui::skins[skin.now].light.back_color;
            lineup.alignment = XY{gui::text::right, gui::text::top};
            lineup.word_wrap = false;
            lineup.style = sys::glyph_style{
                sys::font{"Consolas", h*110/100},
                pix::teal};

            editor.virtual_space = true;
            editor.page.view.word_wrap = false;
            editor.page.view.ground.color = pix::white;
            editor.page.style = sys::glyph_style{
                sys::font{"Consolas", h*110/100},
                pix::black};

            auto s = editor.page.style.now;
            s.color = pix::black;   editor.styles["name"   ] = sys::glyph_style_index(s);
            s.color = pix::blue;    editor.styles["keyword"] = sys::glyph_style_index(s);
            s.color = pix::blue;    editor.styles["pragma" ] = sys::glyph_style_index(s);
            s.color = pix::purple;  editor.styles["macros" ] = sys::glyph_style_index(s);
            s.color = pix::navy;    editor.styles["number" ] = sys::glyph_style_index(s);
            s.color = pix::white;   editor.styles["space"  ] = sys::glyph_style_index(s); 
            s.color = pix::navy;    editor.styles["literal"] = sys::glyph_style_index(s); 
            s.color = pix::navy;    editor.styles["char"   ] = sys::glyph_style_index(s); 
            s.color = pix::maroon;  editor.styles["symbol" ] = sys::glyph_style_index(s);
            s.color = pix::fuchsia; editor.styles["comment"] = sys::glyph_style_index(s);
            s.color = pix::red;     editor.styles["error"  ] = sys::glyph_style_index(s);
        }
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }

    void on_notify (gui::base::widget* w) override
    {
        if (w == &editor)
        {
            int n1 = lineup.model->lines.size();
            int n2 = editor.model.lines.size();
            if (n1 != n2) {
                str text;
                text.reserve(n2*(int)(std::log10(n2)));
                for (int i=0; i<n2; i++)
                    text += std::to_string(i+1) + " \n";
                lineup.text = text;
            }

            notify();
        }
    }
    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &editor.page.scroll.y)
            lineup.shift = XY(0, n);
    }
};
