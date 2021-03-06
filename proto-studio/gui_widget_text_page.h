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
        binary_property<style> style;
        binary_property<bool> word_wrap = true;
        binary_property<bool> ellipsis = false;
        binary_property<XY> alignment = XY{center, center};
        binary_property<XY> margin_right;
        binary_property<XY> margin_left;
        scroll scroll;

        gui::text::view info;

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
                //scroll.x.coord = XYWH();
                //scroll.y.coord = XYWH();
            }
            if((what == &coord && coord.was.size != coord.now.size)
            ||  what == &text
            ||  what == &html
            ||  what == &font
            ||  what == &style
            ||  what == &word_wrap
            ||  what == &margin_right
            ||  what == &margin_left)
                refresh();

            if (what == &timer) {
                if (select_notch < time::now) {
                    select_notch = time::now + select_lapse;
                    int d = gui::metrics::text::height/2;
                    int x = select_point.x, w = coord.now.size.x, dx = 0;
                    int y = select_point.y, h = coord.now.size.y, dy = 0;
                    if (x < 0) dx = x; else if (x > w) dx = x-w;
                    if (y < 0) dy = y; else if (y > h) dy = y-h;
                    dx = dx/d*(int)(log(abs(dx)));
                    dy = dy/d*(int)(log(abs(dy)));
                    if (dx != 0) scroll.x.top = scroll.x.top.now + dx;
                    if (dy != 0) scroll.y.top = scroll.y.top.now + dy;
                    on_mouse_hover(select_point);
                }
            }
        }

        void refresh ()
        {
            XY size = coord.now.size;
            bool
                scroll_x =
                scroll.x.mode == scroll::mode::permanent || (
                scroll.x.mode == scroll::mode::automatic &&
                view.column.coord.now.size.x > size.x );
            bool
                scroll_y =
                scroll.y.mode == scroll::mode::permanent || (
                scroll.y.mode == scroll::mode::automatic &&
                view.column.coord.now.size.y > size.y );
            
            int d = gui::metrics::text::height + 2*gui::metrics::line::width;
            int x = scroll_y ? size.x - d : size.x;
            int y = scroll_x ? size.y - d : size.y;

            scroll.x.show (scroll_x);
            scroll.y.show (scroll_y);

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

        void on_notify (void* what) override
        {
            if (what == &scroll.x) update(); // speed up redraw
            if (what == &scroll.y) update(); // speed up redraw

            if (what == &scroll.x) view.shift = XY(-scroll.x.top, view.shift.now.y);
            if (what == &scroll.y) view.shift = XY(view.shift.now.x, -scroll.y.top);

            if (what == &scroll.x) notify(what);
            if (what == &scroll.y) notify(what);
        }

        void on_mouse_wheel (XY p, int delta) override
        {
            delta /= 20;
            delta *= gui::metrics::text::height;
            int sign = delta < 0 ? -1 : 1;
            if (sys::keyboard::shift) delta = sign * coord.now.h;
            if (sys::keyboard::ctrl) delta *= 5;
            int d = view.coord.now.h - view.column.coord.now.h; // may be negative
            int y = view.shift.now.y + delta;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroll.y.top =-y;
        }

        range point (XY p) {
            return view.column.point(p - 
                view.column.coord.now.origin);
        }

        token* target (XY p) {
            return view.column.target(p - 
                view.column.coord.now.origin);
        }

        bool touch = false; XY touch_point; range touch_range; time touch_time;
        
        property<time> timer;
        time select_delay = time();
        time select_lapse = 100ms;
        time select_notch;
        XY select_point;

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button == 'R') return;
            if (button != 'L') return;
            if (down && !touch)
            {
                if (touch_point == p && time::now - touch_time < 1000ms)
                {
                    //go(-TOKEN); go(+TOKEN, true); // select token
                    //while (caret_upto > 0 &&
                    //    symbol_kind(caret_upto-1) == ' ')
                    //    caret_upto--;
                    //refresh();
                }
                else
                {
                    select_point = p;
                    touch_range = point(p);
                }
                touch_point = p;
                touch_time = time::now;
            }
            touch = down;

            select_notch = time::now + select_delay;
            timer.go (down ? time::infinity : time(),
                      down ? time::infinity : time());

            if (parent)
                parent->on_mouse_press (
                    p + coord.now.origin,
                    button, down);
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
                    select_point = p;
                    range selection;
                    selection.from = touch_range.from;
                    selection.upto = point(p).from; // not point(p).upto !
                    array<range> selections;
                    selections += selection;
                    view.selections = selections;
                    info.hide();
                }
                else
                {
                    if (auto token = target(p); token && token->info != "")
                    {
                        XYWH r = view.column.bar(point(p).from, false);
                        info.hide(); r.w = r.h*100;
                        info.alignment = XY{pix::left, pix::top};
                        info.coord = r;
                        info.html = token->info;
                        r.w = info.column.coord.now.w + r.h*2; r.y += r.h;
                        r.h = info.column.coord.now.h + r.h/2;
                        info.coord = r;
                        info.alignment = XY{pix::center, pix::center};
                        info.show();
                    }
                    else info.hide();
                }
            }

            if (parent)
                parent->on_mouse_hover (
                    p + coord.now.origin);
        }
    };
} 