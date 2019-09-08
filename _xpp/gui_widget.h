#pragma once
#include "pix.h"
#include "gui_effect.h"
namespace gui
{
    struct widget
    {
        property<XYWH>     coord;
        property<RGBA>     color;
        property<uint8_t>  alpha;
        array<XYXY>      updates;
        array<widget*>  children;
        widget* parent = nullptr;
        widget* owner  = nullptr;
        widget* mouse_press_child = nullptr;
        widget* mouse_hover_child = nullptr;

        widget (widget* parent) :
            parent(parent), owner(parent) {
            if (parent) parent->children += this;
            show();
        }
       ~widget() {
           if (parent)
               parent->children.erase(
               parent->children.find(this));
        }

        void update () { update (coord.now - coord.now.origin()); }
        void update (XYWH r) { updates += r; // will be optimized
            if (parent) if (alpha.now != 255 || color.now.a != 255)
                parent->update (r + coord.now.origin());
        }

        XY render_origin;
        //XYXY render_update;
        FRAME<RGBA> render_frame;
        void render (FRAME<RGBA> frame, XY origin) {
            if (alpha.now == 0) return;
            //XYXY rect; for (auto r : updates) rect |= r; updates.clear();
            if (color.now.a != 0) frame.blend(color.now, alpha.now);
            render_frame = frame;//.frame(coord.now);
            render_origin = origin;// + coord.now.origin();
            //render_update = rect;
            on_render(); for (auto w : children)
            w->render(frame.frame(w->coord.now), origin + w->coord.now.origin());
        }
        virtual void on_render () {}

        void fill (XYXY r, RGBA c)
        {
            render_frame.frame(r - render_origin).blend (c, alpha.now);
        }

        void tick ()
        {
            coord.tick(); alpha.tick(); color.tick();

            if (coord.was != coord.now) {
                if (alpha.was != 0 && color.was.a != 0 && parent) parent->update (coord.was);
                if (alpha.now != 0 && color.now.a != 0) update ();
            }
            else
            if (color.was != color.now
            ||  alpha.was != alpha.now) update ();

            on_tick(); for (auto w : children)
            w->tick();
        }
        virtual void on_tick () {}

        virtual void hide (bool off = true) { alpha.go(off? 0 : 255); }
        virtual void show (bool on  = true) { alpha.go(on ? 255 : 0); }

        virtual void move_to (XYWH  r) { coord.go(r); }
        virtual void move_to (XY    p) { coord.go(XYWH(p.x, p.y, coord.to.w, coord.to.h)); }
        virtual void resize  (XY size) { coord.go(XYWH(coord.to.x, coord.to.y, size.x, size.y)); }

        virtual void on_mouse_press (XY, char button, bool down) {}
        virtual void on_mouse_wheel (XY, int) {}
        virtual void on_mouse_hover (XY) {}
        virtual void on_mouse_leave () {}

        virtual bool mouse_sense (XY p) { return alpha.now > 0 && color.now.a > 0 && coord.now.includes(p); }

        virtual void mouse_press (XY p, char button, bool down)
        {
            if (down) {
                mouse_press_child = nullptr;
                for (auto w : children) {
                    if (w->mouse_sense (p - w->coord.now.origin())) {
                        w->mouse_press (p - w->coord.now.origin(), button, true);
                        mouse_press_child = w;
                        return;
                    }
                }
            }
            else { 
                if (mouse_press_child) {
                    mouse_press_child->mouse_press (p - 
                    mouse_press_child->coord.now.origin(), button, false);
                    mouse_press_child = nullptr;
                    return;
                }
            }
            on_mouse_press(p, button, down);
        }

        virtual void mouse_move (XY p)
        {
            if (mouse_press_child) {
                mouse_press_child->mouse_move (p - 
                mouse_press_child->coord.now.origin());
                return;
            }
            widget* hover = nullptr;
            for (auto w : children) {
                if (w->mouse_sense (p - w->coord.now.origin())) {
                    w->mouse_move  (p - w->coord.now.origin());
                    hover = w;
                    return;
                }
            }
            if (mouse_hover_child &&
                mouse_hover_child != hover)
                mouse_hover_child->on_mouse_leave();

            mouse_hover_child = hover;

            if (!hover) on_mouse_hover(p);
        }

        virtual void mouse_wheel (XY p, int delta)
        {
        }
        virtual void mouse_leave ()
        {
        }
    };
}
