#pragma once
#include "gui_widget_canvas.h"
namespace gui
{
    struct button : widget<button>
    {
        property<uint8_t> mouse_hover;
        property<uint8_t> mouse_pressed;
        property<uint8_t> focused;
        property<uint8_t> pressed;
        property<uint8_t> enabled;

        canvas background;
        frame frame_focus;
        frame frame_mouse;

        button(base::widget* parent = nullptr) : widget(parent)
        {
            enabled = 255;
            frame_focus.hide();
            frame_mouse.hide();
            frame_focus.thickness = 2;
            frame_mouse.thickness = 2;
            frame_focus.color = pix::yellow;
            frame_mouse.color = pix::red;
            background .color = pix::maroon;
        }

        bool mouse_sensible (XY p) override { return enabled.to == 255; }

        void on_mouse_press (XY, char button, bool down) override {
            if (enabled.to == 255 && button == 'L') {
                mouse_pressed = down ? 255 : 0;
                pressed = down ? 255 : 0;
                background.color = down ? pix::red : pix::maroon;
                if (down) notify();
            }
        }
        void on_mouse_hover (XY p) override {
            if (enabled.to == 255) {
                mouse_hover = coord.now.local().includes(p) ? 255 : 0;
                pressed = coord.now.local().includes(p) && mouse_pressed.to == 255 ? 255 : 0;
            }
        }
        void on_mouse_leave () override {
            mouse_pressed = 0;
            mouse_hover = 0;
            pressed = 0;
        }

        void on_change () override
        {
            frame_focus.alpha = focused.to;
            frame_mouse.alpha = mouse_hover.to;

            if (coord.was.size() != coord.now.size())
            {
                auto r = coord.now.local();
                frame_focus.move_to(r); r.deflate(frame_focus.thickness.now);
                frame_mouse.move_to(r); r.deflate(frame_mouse.thickness.now);
                background .move_to(r);
            }
        }
    };
}
