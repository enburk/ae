#pragma once
#include "gui_widget.h"
namespace gui
{
    struct splitter : widget<splitter>
    {
        property<int> lower;
        property<int> upper;
        bool touch = false;
        XY touch_origin;
        XY touch_point;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size )
            {
                if (coord.now.size.x > coord.now.size.y)
                    mouse_image = "horizontal splitter"; else
                    mouse_image = "vertical splitter";
            }
        }

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button != 'L') return;
            if (down && !touch) {
                touch_origin = coord.now.origin;
                touch_point = touch_origin + p;
            }
            touch = down;
        }
        void on_mouse_hover (XY p) override
        {
            if (!touch) return;

            if (coord.now.size.x > coord.now.size.y)
            {
                int y = coord.now.origin.y + p.y;
                y = touch_origin.y + y - touch_point.y;
                y = max (y, lower.now);
                y = min (y, upper.now);
                notify(y);
            }
            else
            {
                int x = coord.now.origin.x + p.x;
                x = touch_origin.x + x - touch_point.x;
                x = max (x, lower.now);
                x = min (x, upper.now);
                notify(x);
            }
        }
    };
}