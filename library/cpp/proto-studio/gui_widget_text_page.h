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
            //view.shift = XY (view.shift.now.x, y);
        }
    };
} 