#pragma once
#include "gui_widget_canvas.h"
#include "gui_widget_text_page.h"
namespace gui::text
{
    struct single_line_editor:
    widget<single_line_editor>
    {
        canvas background;
        canvas selection;

        token line;
        caret caret;

        binary_property<str> text;
        binary_property<int> alignment = left;
        binary_property<sys::glyph_style> style;
        sys::glyph_style_index style_index;

        array<str> glyphs;
        int caret_from = 0;
        int caret_upto = 0;
        int shift = 0;

        void on_change (void* what) override
        {
            if (what == &coord && coord.now.size != coord.was.size)
            {
                auto r = coord.now.local();
                background.coord = r;
                refresh();
            }
            if (what == &skin)
            {
                selection.color = skins[skin.now].selection;
                caret.canvas.color = skins[skin.now].touched.back_color;
            }
            if (what == &text) refresh();
            if (what == &alignment) refresh();
            if (what == &style) style_index = sys::glyph_style_index(style.now);
            if (what == &style) refresh();
        }

        void refresh ()
        {
            str junk;
            text.now.split_by("\n", text.now, junk);
            line.fill(text.now + "\n", style_index);
            glyphs = unicode::glyphs(text.now);

            caret_from = aux::clamp(caret_from, 0, glyphs.size());
            caret_upto = aux::clamp(caret_upto, 0, glyphs.size());

            int width = line.coord.now.size.x;
            int Width = coord.now.size.x;

            if (Width < width)
            {
                XYXY r = line(caret_upto).coord.now;
                if (r.right > Width) shift = Width - r.right;
                if (r.left  <     0) shift = 0;
            }
            else
            switch (alignment.now) {
            case left  : shift = 0; break;
            case center: shift = Width/2 - width/2; break;
            case right : shift = Width - width; break;
            }

            line.move_to(XY(shift, coord.now.size.y/2 - line.coord.now.size.y/2));

            caret.coord = line(caret_upto).coord.now + line.coord.now.origin;

            int from = min(caret_from, caret_upto);
            int upto = max(caret_from, caret_upto);
            selection.coord = XYXY (
            XYXY(line(from).coord.now).xl + line.coord.now.origin.x,
            XYXY(line(from).coord.now).yl + line.coord.now.origin.y, 
            XYXY(line(upto).coord.now).xl + line.coord.now.origin.x,
            XYXY(line(upto).coord.now).yh + line.coord.now.origin.y);
        }

        enum WHERE { THERE, GLYPH, TOKEN, BEGIN, END };

        char symbol_kind (int i) const {
            const str & s = glyphs[i];
            return
                s.size() < 1 ? '\0' :
                s.size() > 1 || // UTF-8
                ('0' <= s[0] && s[0] <= '9') ||
                ('A' <= s[0] && s[0] <= 'Z') ||
                ('a' <= s[0] && s[0] <= 'z') ? 'A' :
                s [0] == ' ' ? ' ' : '~';
        }
        void go_to_the_next_token ()
        {
            auto kind = symbol_kind(caret_upto);
            while (caret_upto < glyphs.size() &&
                symbol_kind(caret_upto) == kind)
                caret_upto++;
            while (caret_upto < glyphs.size() &&
                symbol_kind(caret_upto) == ' ')
                caret_upto++;
        }
        void go_to_the_prev_token ()
        {
            int & i = caret_upto;
            int was = caret_upto;
            auto kind = symbol_kind(i);
            while (i > 0 && symbol_kind(i-1) == kind) i--; if (i    != was) return; if (i > 0) i--; kind = symbol_kind(i);
            while (i > 0 && symbol_kind(i-1) == kind) i--; if (kind != ' ') return; if (i > 0) i--; kind = symbol_kind(i);
            while (i > 0 && symbol_kind(i-1) == kind) i--;
        }

        void go (int where, bool selective = false)
        {
            switch(where){
            case THERE: caret_from = caret_upto; break;
            case-GLYPH: caret_upto = clamp(caret_upto-1, 0, glyphs.size()); break;
            case+GLYPH: caret_upto = clamp(caret_upto+1, 0, glyphs.size()); break;
            case-TOKEN: go_to_the_prev_token(); break;
            case+TOKEN: go_to_the_next_token(); break;
            case BEGIN: caret_upto = 0; break;
            case END  : caret_upto = glyphs.size(); break;
            }
            if (!selective) caret_from = caret_upto;
            refresh();
        }

        str selected () const
        {
            int from = min(caret_from, caret_upto);
            int upto = max(caret_from, caret_upto);
            return str(glyphs.from(from).upto(upto), "");
        }
        bool erase_selected ()
        {
            if (caret_from == caret_upto) return false;
            int from = min(caret_from, caret_upto);
            int upto = max(caret_from, caret_upto);
            glyphs.from(from).upto(upto).erase();
            caret_from = caret_upto = from;
            return true;
        }

        void undo () {}
        void redo () {}

        void erase ()
        {
            if (!erase_selected() &&
                 caret_from < glyphs.size())
                 glyphs.erase(caret_from);

            text = str(glyphs, "");
            notify();
        }

        void backspace ()
        {
            if (!erase_selected() && caret_from > 0)
            {
                 caret_from--;
                 caret_upto--;
                 glyphs.erase(caret_from);
            }
            text = str(glyphs, "");
            notify();
        }

        void insert (str s)
        {
            if (!erase_selected() &&
                !caret.insert_mode.now &&
                 caret_from < glyphs.size())
                 glyphs.erase(caret_from);

            glyphs.insert(caret_from, unicode::glyphs(s));
            caret_from += s.size();
            caret_upto += s.size();
            text = str(glyphs, "");
            notify();
        }

        bool touch = false; XY touch_point; time touch_time;

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button == 'R') return; // todo contex menu: cut, copy, paste
            if (button != 'L') return;
            bool drag_and_drop = false;
            if (!drag_and_drop)
            {
                if (down && !touch)
                {
                    if (touch_point == p && time::now - touch_time < 1000ms)
                    {
                        go(-TOKEN); go(+TOKEN, true); // select token
                        while (caret_upto > 0 &&
                            symbol_kind(caret_upto-1) == ' ')
                            caret_upto--;
                        refresh();
                    }
                    else
                    {
                        int i = 0;
                        for (int j=0; j<line.size(); j++)
                            if (line(j).coord.now.x + line.coord.now.origin.x <= p.x)
                                i = j;
                        caret_from = caret_upto = i;
                        refresh();
                    }
                    touch_point = p;
                    touch_time = time::now;
                }
                touch = down;
            }
        }
        void on_mouse_hover (XY p) override
        {
            bool drag_and_drop = false;
            bool inside_selection = false;

            mouse_image = drag_and_drop ?
            inside_selection ? "noway" : "arrow" :
            inside_selection ? "arrow" : "editor";

            if (!drag_and_drop)
            {
                if (touch)
                {
                    int i = 0;
                    for (int j=0; j<line.size(); j++)
                        if (line(j).coord.now.x + line.coord.now.origin.x <= p.x)
                            i = j;
                    caret_upto = i;
                    refresh();
                }
            }
        }

        void on_key_pressed (str key, bool down) override
        {
            if (!down) return;
            if (touch) return; // mouse
            if (key == "space" ) return;
            if (key.size() <= 1) return; // "", "0", "A", ...
            if (key.size() <= 7 && key.starts_with("shift+")) return; // shift+0, shift+A, ...

            if (key == "ctrl+C") key = "ctrl+insert"; else // copy
            if (key == "ctrl+V") key = "shift+insert"; else // paste
            if (key == "ctrl+X") key = "shift+delete"; else // cut
            if (key == "ctrl+Z") key = "alt+backspace"; else // undo

            if (key == "ctrl+Y"      ) key = "ctrl+backspace"; else // redo
            if (key == "ctrl+shift+Z") key = "ctrl+backspace"; else // redo
            {}

            if (key == "left"       ) go(-GLYPH); else
            if (key == "right"      ) go(+GLYPH); else
            if (key == "shift+left" ) go(-GLYPH, true); else
            if (key == "shift+right") go(+GLYPH, true); else

            if (key == "ctrl+left"       ) go(-TOKEN); else
            if (key == "ctrl+right"      ) go(+TOKEN); else
            if (key == "ctrl+shift+left" ) go(-TOKEN, true); else
            if (key == "ctrl+shift+right") go(+TOKEN, true); else

            if (key == "home"      ) go(BEGIN); else
            if (key == "end"       ) go(END  ); else
            if (key == "shift+home") go(BEGIN, true); else
            if (key == "shift+end" ) go(END,   true); else
            if (key == "ctrl+A"  ) { go(BEGIN); go(END, true); } else

            if (key == "insert"      ) { caret.insert_mode = !caret.insert_mode.now; } else
            if (key == "shift+insert") { insert(sys::clipboard::get::string()); } else
            if (key == "ctrl+insert" ) { sys::clipboard::set(selected()); } else

            if (key == "delete"      ) { erase(); } else
            if (key == "shift+delete") { sys::clipboard::set(selected()); erase(); } else

            if (key == "backspace"     ) { backspace(); } else
            if (key == "alt+backspace" ) { undo(); refresh(); } else
            if (key == "ctrl+backspace") { redo(); refresh(); } else // != VS

            if (key == "escape") { go(THERE); } else
            {}
        }
        void on_keyboard_input (str symbol) override
        {
            if (!touch) insert(symbol);
        }
        void on_focus (bool on) override { caret.show(on); }
    };
}