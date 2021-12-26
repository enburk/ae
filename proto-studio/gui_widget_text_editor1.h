#pragma once
#include "gui_widget_text_editor.h"
namespace gui::text
{
    struct one_line_editor:
    widget<one_line_editor>
    {
        canvas canvas;
        editor editor;
        unary_property<str> text;
        property<RGBA> color;
        binary_property<sys::font> font;
        binary_property<style> style;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> margin_right;
        binary_property<XY> margin_left;
        binary_property<bool> insert_mode = true;
        binary_property<bool> virtual_space = false;

        one_line_editor ()
        {
            editor.page.view.ellipsis = false;
            editor.page.view.word_wrap = false;
            editor.page.scroll.x.mode = gui::scroll::mode::none;
            editor.page.scroll.y.mode = gui::scroll::mode::none;
        }

        void on_change (void* what) override
        {
            #define CHANGE(p)          \
            if (what == &p) {          \
                editor.page.p = p.now; \
                p.now = editor.page.p; \
            }
            CHANGE(font)
            CHANGE(color)
            CHANGE(style)
            CHANGE(alignment)
            CHANGE(margin_right)
            CHANGE(margin_left)
            #undef CHANGE
            #define CHANGE(p)     \
            if (what == &p) {     \
                editor.p = p.now; \
                p.now = editor.p; \
            }
            CHANGE(insert_mode)
            CHANGE(virtual_space)
            #undef CHANGE

            if (what == &coord)
            {
                canvas.coord = coord.now.local();
                editor.coord = coord.now.local();
            }
            if (what == &text)
            {
                editor.model->set(doc::text::text(text));
                editor.reset();
            }
            if (what == &editor)
            {
                text.now = editor.model->string();
                notify();
            }
        }

        void select ()
        {
            editor.go(gui::text::editor::TEXT_BEGIN, false);
            editor.go(gui::text::editor::TEXT_END, true);
            editor.page.view.refresh();
            editor.page.refresh();
            editor.refresh();
        }

        void on_key_pressed (str key, bool down) override
        {
            if (!down) return;

            if (key == "ctrl+C") key = "ctrl+insert"; else // copy
            if (key == "ctrl+V") key = "shift+insert"; else // paste
            if (key == "ctrl+X") key = "shift+delete"; else // cut
            if (key == "ctrl+Z") key = "alt+backspace"; else // undo

            if (key == "shift+insert"     )
            {
                str s = sys::clipboard::get::string();
                s.replace_all("\n", " ");
                editor.insert(s);
            }
            else

            if (key == "enter"            ) {} else
            if (key == "shift+enter"      ) {} else
            if (key == "ctrl+enter"       ) {} else
            if (key == "ctrl+shift+enter" ) {} else

            if (key == "tab"              ) {} else
            if (key == "shift+tab"        ) {} else

            editor.on_key_pressed(key, down);
        }
        void on_keyboard_input (str symbol) override
        {
            editor.on_keyboard_input(symbol);
        }
        void on_focus (bool on) override
        {
            editor.on_focus(on);
        }
    };
}