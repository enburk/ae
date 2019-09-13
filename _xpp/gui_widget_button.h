#pragma once
#include "gui_widget_frame.h"
namespace gui
{
    struct button : widget
    {
        property<uint8_t> mouse_hover;
        property<uint8_t> mouse_pressed;
        property<uint8_t> focused;
        property<uint8_t> pressed;
        property<uint8_t> enabled;

        gui::frame frame_focus = gui::frame(this);
        gui::frame frame_mouse = gui::frame(this);

        button(gui::widget* parent) : gui::widget(parent)
        {
            frame_focus.hide();
            frame_mouse.hide();
            frame_focus.thickness.go(2);
            frame_mouse.thickness.go(2);
            frame_focus.color.go(pix::yellow);
            frame_mouse.color.go(pix::red);
            widget::    color.go(pix::maroon);
        }

        bool mouse_sense (XY p) override { return alpha.now > 0 && coord.now.includes(p); }

        void on_mouse_press (XY, char button, bool down) override {
            if (enabled.to == 255 && button == 'L') {
                mouse_pressed.go(down ? 255 : 0);
                pressed.go(down ? 255 : 0);
                if (down) notify();
            }
        }
        void on_mouse_hover (XY p) override {
            if (enabled.to == 255) {
                mouse_hover.go((coord.now - coord.now.origin()).includes(p) ? 255 : 0);
                pressed.go((coord.now - coord.now.origin()).includes(p) && mouse_pressed.to == 255 ? 255 : 0);
            }
        }
        void on_mouse_leave () override {
            mouse_pressed.go(0);
            mouse_hover.go(0);
            pressed.go(0);
        }

        void on_tick () override
        {
            mouse_hover  .tick();
            mouse_pressed.tick();
            focused.tick();
            pressed.tick();
            enabled.tick();

            frame_focus.alpha.go(focused.to);
            frame_mouse.alpha.go(mouse_hover.to);

            if (coord.was.size() != coord.now.size())
            {
                auto r = coord.now - coord.now.origin();
                frame_focus.move_to(r); r.deflate(frame_focus.thickness.now);
                frame_mouse.move_to(r); r.deflate(frame_mouse.thickness.now);
            }
        }
    };
}
