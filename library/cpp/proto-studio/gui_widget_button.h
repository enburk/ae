#pragma once
#include "gui_colors.h"
#include "gui_widget_canvas.h"
#include "gui_widget_image.h"
#include "gui_widget_text_view.h"
namespace gui
{
    struct button:
    widget<button>
    {
        binary_property<bool> on = false;
        binary_property<bool> enabled = true;
        binary_property<bool> focused = false;
        binary_property<bool> mouse_hover = false;
        binary_property<bool> mouse_pressed = false;
        binary_property<bool> enter_pressed = false;
        enum {normal, toggle, sticky} kind;
        bool repeating = false;
        time repeat_delay = 500ms;
        time repeat_lapse = 100ms;
        time repeat_notch;
        property<time> timer;

        frame frame; image image; text::view text;

        std::function<void(void)> on_change_state = [this]()
        {
            auto style = skins[skin.now];

            frame.color = style.focus.back_color;
            frame.alpha.go (focused ? 255 : 0);

            auto colors = style.normal;

            if (!enabled     .now) colors = style.disabled; else
            if (mouse_pressed.now) colors = style.touched; else
            if (enter_pressed.now) colors = style.touched; else
            if (on           .now) colors = style.active; else
            if (mouse_hover  .now) colors = style.hovered;

            text.canvas.color.go(colors.back_color);
            text.color.go(colors.fore_color);

            auto r = coord.now.local();
            frame.thickness = metrics::line::width;
            frame.coord = r; r.deflate(frame.thickness.now);
            image.coord = r; r.deflate(frame.thickness.now);
            text .coord = r;
        };

        void on_change (void* what) override
        {
            if (what == &enter_pressed) on_change_state(); else
            if (what == &mouse_pressed) on_change_state(); else
            if (what == &mouse_hover) on_change_state(); else
            if (what == &focused) on_change_state(); else
            if (what == &enabled) on_change_state(); else
            if (what == &coord) on_change_state(); else
            if (what == &skin) on_change_state(); else
            if (what == &on) { on_change_state(); notify(); }

            if (what == &timer) {
                if (repeat_notch < time::now) {
                    repeat_notch = time::now + repeat_lapse;
                    if (mouse_pressed.now) notify();
                }
            }
        }

        bool mouse_sensible (XY) override { return enabled.now; }
        void on_mouse_hover (XY) override { mouse_hover = true; }
        void on_mouse_leave (  ) override { mouse_hover = false;}
        void on_mouse_press (XY, char button, bool down) override
        {
            if (button != 'L') return;
            
            mouse_pressed = down;
            
            if (enabled.now) {
                switch(kind) {
                case normal: on = down; break;
                case toggle: on = down || on.was != on.now; break;
                case sticky: if (down) on = true; break;
                }
            }
            if (repeating) {
                repeat_notch = time::now + repeat_delay;
                timer.go (down ? time::infinity : time(),
                          down ? time::infinity : time());
            }
        }
    };

    struct checkbox : button
    {
        checkbox ()
        {

            on_change_state = [this]()
            {
            };
        }
    };

    struct radio
    {
        struct button : gui::widget<button>
        {
            gui::button sensor;

            button ()
            {
                sensor.kind = gui::button::sticky;
            }
        };

        struct group : gui::widget<group>
        {
            gui::widgetarium<button> buttons;

            void on_notify (gui::base::widget* w) override
            {
                for (auto & b : buttons) b.sensor.on = &b == w; notify();
            }
        };
    };
}
