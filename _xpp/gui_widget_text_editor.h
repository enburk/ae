#pragma once
#include "gui_widget_text_view.h"
namespace gui::text
{
    struct editor final : widget<editor>
    {
        struct caret final : widget<caret>
        {
            canvas canvas;
            property<time> timer;
            binary_property<bool> insert_mode = true;
            void on_change (void* what) override
            {
                if (timer.now == time())
                    timer.go (time::infinity,
                              time::infinity);

                if (what == &coord && coord.now.size != coord.was.size
                ||  what == &insert_mode) {
                    XYWH r = coord.now.local();
                    if (insert_mode.now) r.w = max (1, r.h/16);
                    canvas.coord = r;
                }
                if (what == &timer) {
                    int ms = time::now.ms % 1024;
                    if (ms > 512) ms = 1024-ms-1;
                    canvas.alpha = 255*ms/512;
                }
            }
        };

        doc::Document _document;
        doc::Document* document = &_document;

        struct model
        {
            XY shift;
            struct caret { int position = 0, selection = 0; };
            array <caret> carets = {caret()};
        }
        model;

        canvas background;
        widgetarium<canvas> highlight;
        widgetarium<canvas> selection;
        view view; // after selection, before carets
        widgetarium<caret> carets;

        binary_property<bool> insert_mode = true;
        binary_property<bool> virtual_space = false;

        void on_change (void* what) override
        {
            if (what == &coord && coord.now.size != coord.was.size)
            {
                auto r = coord.now.local();
                background.coord = r;
                highlight.coord = r;
                selection.coord = r;
                carets.coord = r;
                view.coord = r;

                if (carets.size() == 0)
                    carets.emplace_back();

                auto co = schemas[""].touched.back_color; co.a = 192;
                carets(0).canvas.color = co;
                carets(0).coord = XYWH(0,0,15,30);
                carets(0).insert_mode = false;
            }
            if (what == &insert_mode)
            {
                for (auto & caret : carets) caret.insert_mode = insert_mode.now;
            }
        }

        void refresh ()
        {
            view.tokens = document->lexica;
        }

        void insert (str s) {
            for (auto & caret : model.carets) {
                document->insert(caret.position, s);
                caret.position += s.size();
            }
            refresh();
        }
        void replace (int n, str s) {
            for (auto & caret : model.carets) {
                document->replace(caret.position, n, s);
                caret.position += s.size();
            }
            refresh();
        }
        void on_focus (bool on) override
        {
            for (auto & caret : carets) caret.show(on);
        }
        void on_key_pressed (str key, bool down) override
        {
            if (!down) return;
            if (key.size() <= 1) return; // "", "0", "A", ...
            if (key.size() <= 7 && key.starts_with("shift+")) return; // shift+0, shift+A, ...
            if (key == "space" ) return;

            if (key == "ctrl+C") key = "ctrl+insert"; else
            if (key == "ctrl+V") key = "shift+insert"; else
            {}

            if (key == "enter" ) insert("\n"); else
            if (key == "insert") insert_mode = !insert_mode.now; else
            {
                insert(" " + key + " "); //(down ? " (down) " : " (up) ");
            }
        }
        void on_keyboard_input (str symbol) override
        {
            if (insert_mode.now)
                insert(symbol); else
                replace(1, symbol);
        }
    };
} 

//    if (0x30 <= wparam && wparam <= 0x39) s = '0' + (char)wparam-0x30; else
//    if (0x41 <= wparam && wparam <= 0x5A) s = 'A' + (char)wparam-0x41; else
//    if (0x70 <= wparam && wparam <= 0x7B) s = "F" + std::to_string(wparam-0x70+1); else
//    switch(wparam){
//    case VK_BACK    : s = "backspace"; break;
//    case VK_TAB     : s = "tab"; break;
//    case VK_RETURN  : s = "enter"; break;
//    case VK_ESCAPE  : s = "escape"; break;
//    case VK_INSERT  : s = "insert"; break;
//    case VK_DELETE  : s = "delete"; break;
//    case VK_SPACE   : s = "space"; break;
//    case VK_SNAPSHOT: s = "print screen"; break;
//
//    case VK_PRIOR   : s = "page up"; break;
//    case VK_NEXT    : s = "page down"; break;
//    case VK_END     : s = "end"; break;
//    case VK_HOME    : s = "home"; break;
//    case VK_LEFT    : s = "left"; break;
//    case VK_RIGHT   : s = "right"; break;
//    case VK_UP      : s = "up"; break;
//    case VK_DOWN    : s = "down"; break;
//
//    case VK_OEM_PLUS    : s = "+"; break;
//    case VK_OEM_MINUS   : s = "-"; break;
//    case VK_OEM_COMMA   : s = ","; break;
//    case VK_OEM_PERIOD  : s = "."; break;
