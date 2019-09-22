#pragma once
#include "pix.h"
#include "gui_layer.h"
namespace gui::base
{
    struct widget : polymorphic, layer
    {
        array<widget*>  children;
        widget* parent = nullptr;
        widget* mouse_press_child = nullptr;
        widget* mouse_hover_child = nullptr;

        widget (widget* parent) :
            parent(parent) {
            if (parent) parent->children += this;
            show();
        }
       ~widget() {
           if (parent)
               parent->children.erase(
               parent->children.find(this));
        }

        void update (      ) { update (coord.now.local()); }
        void update (XYWH r) { updates += r; if (parent) parent->update (r + coord.now.origin()); }

        // void update () { update (coord.now - coord.now.origin()); }
        // void update (XYWH r) { updates += r; // will be optimized
        //     if (parent) if (alpha.now != 255 || color.now.a != 255)
        //         parent->update (r + coord.now.origin());
        // }


        FRAME<RGBA> render_window_frame; XY render_local_origin; void render ()
        {
            if (alpha.now   == 0) return;
            if (color.now.a != 0) render_window_frame.blend(color.now, alpha.now);

            //layer::render()

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

        virtual void hide (bool off = true) { alpha = off? 0 : 255; }
        virtual void show (bool on  = true) { alpha = on ? 255 : 0; }

        virtual void move_to (XYWH  r) { coord = r; }
        virtual void move_to (XY    p) { coord = XYWH(p.x, p.y, coord.to.w, coord.to.h); }
        virtual void resize  (XY size) { coord = XYWH(coord.to.x, coord.to.y, size.x, size.y); }

        virtual bool mouse_sensible (XY p) { return false; }
        virtual void on_mouse_press (XY, char button, bool down) {}
        virtual void on_mouse_wheel (XY, int) {}
        virtual void on_mouse_hover (XY) {}
        virtual void on_mouse_leave () {}

        bool mouse_sense (XY p)
        {
            if (alpha.now == 0 || coord.now.local().excludes(p)) return false;
            for (auto w : children)
                if (w->mouse_sense (p - w->coord.now.origin()))
                    return true;
            return mouse_sensible(p);
        }

        void mouse_press (XY p, char button, bool down)
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

        void mouse_move (XY p)
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
                }
            }
            if (mouse_hover_child &&
                mouse_hover_child != hover)
                mouse_hover_child->on_mouse_leave();

            mouse_hover_child = hover;

            if (!hover) on_mouse_hover(p);
        }

        void mouse_wheel (XY p, int delta)
        {
        }
        void mouse_leave ()
        {
        }

        void notify () { if (parent) parent->on_notify(this); }
        virtual void on_notify (widget*) {}

        size_t size;
    };
}
namespace gui
{
    inline array<base::widget*> widgets;

    base::widget* owner (void* ptr, array<base::widget*> & ww = widgets)
    {
        for (base::widget* w : ww) {
            std::byte* that = (std::byte*) w;
            std::byte* next = that + w->size;
            if (that <= ptr && ptr < next) {
                base::widget* parent = owner (ptr, w->children);
                if (!parent) parent = w;
                return parent;
            }
        }
        return nullptr;
    }

    template<class T> struct widget : base::widget
    {
        widget (base::widget* p = nullptr) : base::widget(p)
        {
            size = sizeof(T);

            if (p == nullptr) p = owner(this);

            if (p) { parent = p; parent->children += this; }

            else widgets += this;
        }
    };
}
