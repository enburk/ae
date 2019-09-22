#pragma once
#include "pix.h"
#include "gui_effect.h"
namespace gui
{
    struct layer
    {
        property<XYWH>    coord;
        property<RGBA>    color;
        property<uint8_t> alpha;
        IMAGE<RGBA>       image;
        rectifier       updates;
        rectifier parent_updates;

        void hide (bool off = true) { alpha = off? 0 : 255; }
        void show (bool on  = true) { alpha = on ? 255 : 0; }

        void move_to (XYWH  r) { coord = r; }
        void move_to (XY    p) { coord = XYWH(p.x, p.y, coord.to.w, coord.to.h); }
        void resize  (XY size) { coord = XYWH(coord.to.x, coord.to.y, size.x, size.y); }

        void render (FRAME<RGBA> frame, XYWH rect)
        {
            if (alpha.now   == 0) return;
            if (color.now.a != 0) frame.frame(XYWH()).blend(color.now, alpha.now);

            on_render();

            XY global_origin = render_window_frame.origin + render_local_origin;

            for (auto child : children) {
                XYWH child_global = child->coord.now + global_origin;
                child->render_window_frame = render_window_frame.frame(child_global - render_window_frame.origin);
                child->render_local_origin = child->render_window_frame.origin - child_global.origin();
                child->render();
            }
        }
        virtual void on_render () {}

        void fill (XYXY local_rect, RGBA c)
        {
            if (alpha.now == 0 || c.a == 0) return;
            render_window_frame.frame(local_rect - render_local_origin).blend (c, alpha.now);
        }

        void change ()
        {
            if (coord.was != coord.now)
            {
                if (alpha.was != 0 && color.was.a != 0 && parent) parent->update (coord.was);
                if (alpha.now != 0 && color.now.a != 0) update ();
            }
            else
            if (color.was != color.now
            ||  alpha.was != alpha.now) update ();

            on_change(); for (auto w : children)
            w->change();
        }
        virtual void on_change () {}
    };
}
