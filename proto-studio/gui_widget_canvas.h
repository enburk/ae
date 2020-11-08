#pragma once
//#include "gui_colors.h"
#include "gui_widget.h"
namespace gui
{
    struct canvas:
    widget<canvas>
    {
        property<RGBA> color;

        Opacity opacity () override { return
            color.now.a == 255 ? opaque :
            color.now.a == 0 ? transparent :
                             semitransparent; }

        void on_render (sys::frame frame, XY offset, uint8_t alpha) override
        {
            frame.blend(frame, color.now, alpha);
        }

        void on_change (void* what) override
        {
            if (what == &color)
                if (color.was.a != 0 ||
                    color.now.a != 0 ) update();
        }
    };

    struct frame:
    widget<frame>
    {
        property<RGBA> color;
        property<int> thickness = gui::metrics::line::width;

        canvas l, t, r, b; // left, top, right, bottom

        void on_change () override
        {
            int w = coord.now.w;
            int h = coord.now.h;
            int d = thickness.now;
            t.coord = XYWH(0, 0,   w, d);
            b.coord = XYWH(0, h-d, w, d);
            l.coord = XYWH(0,   d, d, h-d-d);
            r.coord = XYWH(w-d, d, d, h-d-d);
            t.color = color.now;
            b.color = color.now;
            l.color = color.now;
            r.color = color.now;
        }
    };

    template<class Object> struct area : widget<area<Object>>
    {
        frame  frame1;
        frame  frame2;
        frame  frame3;
        Object object;

        using widget<area<Object>>::skin;
        using widget<area<Object>>::coord;
        using widget<area<Object>>::notify;

        void on_change (void* what) override
        {
            if (what == &skin)
            {
                auto & style = gui::skins[skin.now];
                frame1.color = style.light.first;
                frame2.color = style.heavy.first;
                frame3.color = style.light.first;
            }
            if (what == &coord && coord.was.size != coord.now.size)
            {
                auto r = coord.now.local();
                frame1.coord = r; r.deflate(frame1.thickness.now);
                frame2.coord = r; r.deflate(frame2.thickness.now);
                frame3.coord = r; r.deflate(frame3.thickness.now);
                object.coord = r;
            }
        }

        void on_focus (bool on) override { object.on_focus(on); }
        void on_keyboard_input (str symbol) override { object.on_keyboard_input(symbol); }
        void on_key_pressed (str key, bool down) override { object.on_key_pressed(key,down); }
        void on_notify (void* what) override { notify(what); }
    };

    struct splitter : widget<splitter>
    {
        property<int> lower;
        property<int> upper;
        bool touched = false;
        XY touch_point;
        int middle = 0;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size )
                mouse_image = coord.now.size.x > coord.now.size.y ?
                    "horizontal splitter" : "vertical splitter";
        }

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button != 'L') return;
            if (down && !touched) touch_point = p;
            touched = down;
        }
        void on_mouse_hover (XY p) override
        {
            if (!touched) return;

            if (coord.now.size.x > coord.now.size.y)
            {
                int y = coord.now.origin.y + p.y - touch_point.y;
                y = max (y, lower.now);
                y = min (y, upper.now);
                middle = y + coord.now.h/2;
                notify();
            }
            else
            {
                int x = coord.now.origin.x + p.x - touch_point.x;
                x = max (x, lower.now);
                x = min (x, upper.now);
                middle = x + coord.now.w/2;
                notify();
            }
        }
    };
}
