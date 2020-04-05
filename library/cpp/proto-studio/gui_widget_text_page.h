#pragma once
#include "gui_widget.h"
#include "gui_widgetarium.h"
#include "gui_widget_text_view.h"
#include "gui_widget_scroller.h"
namespace gui::text
{
    struct page:
    widget<page>
    {
        view view;
        unary_property<str> text;
        unary_property<str> html;
        property<RGBA> color;
        binary_property<sys::font> font;
        binary_property<glyph_style> style;
        binary_property<bool> word_wrap = true;
        binary_property<bool> ellipsis = false;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> margin_right;
        binary_property<XY> margin_left;
        struct scroll
        {
            enum class mode { none, automatic, permanent };
            struct X : scroller<horizontal> { mode mode = mode::automatic; }; X x;
            struct Y : scroller<vertical>   { mode mode = mode::automatic; }; Y y;
        }
        scroll;

        void on_change (void* what) override
        {
            #define CHANGE(p) if (what == &p) view.p = p.now;
            CHANGE(text)
            CHANGE(html)
            CHANGE(font)
            CHANGE(color)
            CHANGE(style)
            CHANGE(word_wrap)
            CHANGE(ellipsis)
            CHANGE(alignment)
            CHANGE(margin_right)
            CHANGE(margin_left)
            #undef CHANGE

            if (what == &text
            ||  what == &html)
            {
                text.now = view.text.now;
                html.now = view.html.now;
            }
            if (what == &skin
            ||  what == &font
            ||  what == &color
            ||  what == &style)
            {
                font .now = view.style.now.font;
                color.now = view.style.now.color;
                style.now = view.style.now;
            }
            if (what == &coord && coord.was.size != coord.now.size)
            {
                view.coord = coord.now.local();
                scroll.x.coord = XYWH();
                scroll.y.coord = XYWH();
            }
            if (what == &coord && coord.was.size != coord.now.size
            ||  what == &text
            ||  what == &html
            ||  what == &font
            ||  what == &style
            ||  what == &word_wrap
            ||  what == &margin_right
            ||  what == &margin_left)
                refresh();
        }

        void refresh ()
        {
            XY size = coord.now.size;
            bool
                scroll_x =
                scroll.x.mode == scroll::mode::permanent ||
                scroll.x.mode == scroll::mode::automatic &&
                view.column.coord.now.size.x > size.x;
            bool
                scroll_y =
                scroll.y.mode == scroll::mode::permanent ||
                scroll.y.mode == scroll::mode::automatic &&
                view.column.coord.now.size.y > size.y;
            
            int d = gui::metrics::text::height + 2*gui::metrics::line::width;
            int x = scroll_x ? scroll_y ? size.x - d : size.x : 0;
            int y = scroll_y ? scroll_x ? size.y - d : size.y : 0;

            scroll.x.coord = XYWH(0, size.y-d, x, d);
            scroll.y.coord = XYWH(size.x-d, 0, d, y);

            scroll.x.span = view.column.coord.now.size.x;
            scroll.y.span = view.column.coord.now.size.y;

            scroll.x.step = gui::metrics::text::height;
            scroll.y.step = gui::metrics::text::height;

            if (scroll_x) size.y -= d;
            if (scroll_y) size.x -= d;

            view.coord = XYWH(0,0,size.x, size.y);
        }

        void on_notify (gui::base::widget* w, int n) override
        {
            if (w == &scroll.x) view.shift = XY(-n, view.shift.now.y);
            if (w == &scroll.y) view.shift = XY(view.shift.now.x, -n);
        }

        void on_mouse_wheel (XY p, int delta) override
        {
            if (sys::keyboard::shift) delta *= coord.now.size.y;
            if (sys::keyboard::ctrl) delta *= 5;
            int d = coord.now.size.y - view.column.coord.now.size.y; // may be negative
            int y = view.shift.now.y + gui::metrics::text::height * delta/120;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroll.y.top =-y;
        }

        bool touch = false; XY touch_point; time touch_time;

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button == 'R') return; // todo contex menu: cut, copy, paste
            if (button != 'L') return;
            if (down && !touch)
            {
                //if (touch_point == p && time::now - touch_time < 1000ms)
                //{
                //    go(-TOKEN); go(+TOKEN, true); // select token
                //    while (caret_upto > 0 &&
                //        symbol_kind(caret_upto-1) == ' ')
                //        caret_upto--;
                //    refresh();
                //}
                //else
                //{
                //    int i = 0;
                //    for (int j=0; j<line.size(); j++)
                //        if (line(j).coord.now.x + line.coord.now.origin.x <= p.x)
                //            i = j;
                //    caret_from = caret_upto = i;
                //    refresh();
                //}
                touch_point = p;
                touch_time = time::now;
            }
            touch = down;
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
                    //int i = 0;
                    //for (int j=0; j<line.size(); j++)
                    //    if (line(j).coord.now.x + line.coord.now.origin.x <= p.x)
                    //        i = j;
                    //caret_upto = i;
                    //refresh();
                }
            }
        }

        void on_key_pressed (str key, bool down) override
        {
            //if (!down) return;
            //if (touch) return; // mouse
            //if (key == "space" ) return;
            //if (key.size() <= 1) return; // "", "0", "A", ...
            //if (key.size() <= 7 && key.starts_with("shift+")) return; // shift+0, shift+A, ...
            //
            //if (key == "ctrl+C") key = "ctrl+insert"; else // copy
            //if (key == "ctrl+V") key = "shift+insert"; else // paste
            //if (key == "ctrl+X") key = "shift+delete"; else // cut
            //if (key == "ctrl+Z") key = "alt+backspace"; else // undo
            //
            //if (key == "ctrl+Y"      ) key = "ctrl+backspace"; else // redo
            //if (key == "ctrl+shift+Z") key = "ctrl+backspace"; else // redo
            //{}
            //
            //if (key == "left"       ) go(-GLYPH); else
            //if (key == "right"      ) go(+GLYPH); else
            //if (key == "shift+left" ) go(-GLYPH, true); else
            //if (key == "shift+right") go(+GLYPH, true); else
            //
            //if (key == "ctrl+left"       ) go(-TOKEN); else
            //if (key == "ctrl+right"      ) go(+TOKEN); else
            //if (key == "ctrl+shift+left" ) go(-TOKEN, true); else
            //if (key == "ctrl+shift+right") go(+TOKEN, true); else
            //
            //if (key == "home"      ) go(BEGIN); else
            //if (key == "end"       ) go(END  ); else
            //if (key == "shift+home") go(BEGIN, true); else
            //if (key == "shift+end" ) go(END,   true); else
            //
            //if (key == "insert"      ) { caret.insert_mode = !caret.insert_mode.now; } else
            //if (key == "shift+insert") { insert(sys::clipboard::get::string()); } else
            //if (key == "ctrl+insert" ) { sys::clipboard::set(selected()); } else
            //
            //if (key == "delete"      ) { erase(); } else
            //if (key == "shift+delete") { sys::clipboard::set(selected()); erase(); } else
            //
            //if (key == "backspace"     ) { backspace(); } else
            //if (key == "alt+backspace" ) { undo(); refresh(); } else
            //if (key == "ctrl+backspace") { redo(); refresh(); } else // != VS
            //
            //if (key == "escape") { go(THERE); } else
            //{}
        }
    };
} 