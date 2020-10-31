#pragma once
#include "gui_colors.h"
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

        void on_render (pix::frame<RGBA> frame, XY offset, uint8_t alpha) override
        {
            frame.blend(color.now, alpha);
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
        canvas canvas;
        frame  frame1;
        frame  frame2;
        Object object;

        using widget<area<Object>>::skin;
        using widget<area<Object>>::coord;
        using widget<area<Object>>::notify;

        area () { on_change(&skin); }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                auto r = coord.now.local();
                frame1.coord = r; r.deflate(frame1.thickness.now);
                frame2.coord = r; r.deflate(frame2.thickness.now);
                canvas.coord = r; r.deflate(frame2.thickness.now);
                object.coord = r;
            }
            if (what == &skin)
            {
                frame1.color = gui::skins[skin.now].light.back_color;
                frame2.color = gui::skins[skin.now].heavy.back_color;
                canvas.color = gui::skins[skin.now].light.back_color;
            }
        }

        void on_focus (bool on) override { object.on_focus(on); }
        void on_keyboard_input (str symbol) override { object.on_keyboard_input(symbol); }
        void on_key_pressed (str key, bool down) override { object.on_key_pressed(key,down); }

        void on_notify (gui::base::widget* w) override { notify(w == &object ? this : w); }
        void on_notify (gui::base::widget* w, int n) override { notify(w == &object ? this : w, n); }
    };

}
