#pragma once
#include <any>
#include <set>
#include "../aux_utils.h"
#include "../pix_image.h"
#include "gui_effect.h"
namespace gui::base
{
    struct widget : polymorphic
    {
        widget* parent = nullptr;
        array  <widget*> children;
        widget (widget &&) = delete;
        widget (const widget &) = delete;
        widget (widget* parent) : parent(parent)
                  { if (parent) parent->children += this; }
       ~widget () { if (parent) parent->children.try_erase(this); }

        ////////////////////////////////////////////////////////////////////////

        property<XYWH> coord; property<uint8_t> alpha = 255; XY advance, baseline;

        void hide    (bool off, time t=time()) { alpha.go(off? 0 : 255, t); }
        void show    (bool on , time t=time()) { alpha.go(on ? 255 : 0, t); }
        void hide    (          time t=time()) { hide(true, t); }
        void show    (          time t=time()) { show(true, t); }
        void move_to (XYWH  r,  time t=time()) { coord.go(r, t); }
        void move_to (XY    p,  time t=time()) { coord.go(XYWH(p.x, p.y, coord.to.w, coord.to.h), t); }
        void shift   (XY    d,  time t=time()) { coord.go(XYWH(coord.to.x+d.x, coord.to.y+d.y, coord.to.w, coord.to.h), t); }
        void resize  (XY size,  time t=time()) { coord.go(XYWH(coord.to.x, coord.to.y, size.x, size.y), t); }

        virtual void on_render (pix::frame<RGBA> frame, XY offset, uint8_t alpha) {}
        virtual void on_change (void* what) { on_change(); }
        virtual void on_change () {}

        enum Opacity { transparent, semitransparent, opaque };
        virtual Opacity opacity () { return transparent; }

        rectifier updates;
        void update () { update (coord.now.local()); }
        void update (XYWH r) {
            if (alpha.now == 0) return; r &= coord.now.local();
            if (parent) parent->update (r +  coord.now.origin);
            else updates += r;
        }

        void change (void* what) {
            if (what == &coord && parent) parent->update(coord.was);
            if (what == &coord && parent) parent->update(coord.now);
            if (what == &alpha && parent) parent->update(coord.now);
            on_change(what);
        }

        void render (pix::frame<RGBA> frame, XY offset, uint8_t combined_alpha = 255)
        {
            combined_alpha = ((combined_alpha+1) * alpha.now) >> 8; if (combined_alpha == 0) return;
            on_render (frame, offset, combined_alpha);
            for (auto child : children) {
                XYWH  child_global = child->coord.now + frame.offset - offset;
                auto  child_frame = frame.crop(child_global - frame.offset);
                if   (child_frame.size.x <= 0) continue;
                if   (child_frame.size.y <= 0) continue;
                child->render(
                    child_frame,
                    child_frame.offset - child_global.origin,
                    combined_alpha);
            }
        }

        ////////////////////////////////////////////////////////////////////////

        widget* mouse_press_child = nullptr;
        widget* mouse_hover_child = nullptr;

        unary_property<str> mouse_image;
        virtual bool mouse_sensible (XY p) { return false; }
        virtual void on_mouse_press (XY, char button, bool down) {}
        virtual void on_mouse_wheel (XY, int) {}
        virtual void on_mouse_hover (XY) {}
        virtual void on_mouse_leave () {}

        bool mouse_sense (XY p)
        {
            if (alpha.now == 0) return false;
            if (coord.now.local().excludes(p)) return false;
            for (auto w : children)
                if (w->mouse_sense (p - w->coord.now.origin))
                    return true;
            return mouse_sensible(p);
        }

        void mouse_press (XY p, char button, bool down)
        {
            // button ?
            if (down) {
                mouse_press_child = nullptr;
                for (auto w : children) {
                    if (w->mouse_sense (p - w->coord.now.origin)) {
                        w->mouse_press (p - w->coord.now.origin, button, true);
                        mouse_press_child = w;
                        return;
                    }
                }
            }
            else { 
                if (mouse_press_child) {
                    mouse_press_child->mouse_press (p - 
                    mouse_press_child->coord.now.origin, button, false);
                    mouse_press_child = nullptr;
                    return;
                }
            }
            on_mouse_press(p, button, down);
            sys::mouse::cursor(mouse_image.now);
        }

        void mouse_move (XY p)
        {
            if (mouse_press_child) {
                mouse_press_child->mouse_move (p - 
                mouse_press_child->coord.now.origin);
                return;
            }
            widget* hover = nullptr;
            for (auto w : children) {
                if (w->mouse_sense (p - w->coord.now.origin)) {
                    w->mouse_move  (p - w->coord.now.origin);
                    hover = w;
                    break;
                }
            }
            if (mouse_hover_child &&
                mouse_hover_child != hover)
                mouse_hover_child->mouse_leave();

                mouse_hover_child = hover;
            if (mouse_hover_child) return;
            
            sys::mouse::cursor(mouse_image.now);
            on_mouse_hover(p); // every mouse_move
        }

        void mouse_leave ()
        {
            if (mouse_hover_child) {
                mouse_hover_child->mouse_leave ();
                mouse_hover_child = nullptr;
            }
            else on_mouse_leave ();
        }

        void mouse_wheel (XY p, int delta)
        {
        }

        ////////////////////////////////////////////////////////////////////////

        virtual void on_focus (bool) {}
        virtual void on_key_pressed (str, bool) {}
        virtual void on_keyboard_input (str) {}

        ////////////////////////////////////////////////////////////////////////

        void notify () { if (parent) parent->on_notify(this); }
        void notify (int i) { if (parent) parent->on_notify(this, i); }

        virtual void on_notify (widget*) {}
        virtual void on_notify (widget*, int) {}

        ////////////////////////////////////////////////////////////////////////

        size_t size_in_bytes;
        bool inholds (void* p) {
            std::byte* begin = (std::byte*) this;
            std::byte* end = begin + size_in_bytes;
            return begin <= p && p < end;
        }
        widget* descendant_inholder(void* p) {
            if (!inholds(p)) return nullptr;
            for (auto child : children) {
                auto descendant = child->descendant_inholder(p);
                if (descendant) return descendant; }
            return this;
        }
    };
}
namespace gui
{
    inline void change (base::widget* widget, void* what) {
        widget->change (what);
    }

    inline std::set<base::widget*> widgets;

    inline base::widget* inholder (void* p) {
        auto it = widgets.upper_bound((base::widget*)p);
        if (it == widgets.begin()) return nullptr;
        return (*--it)->descendant_inholder(p);
    }

    template<class T> struct widget : base::widget
    {
        widget (base::widget* p = nullptr) : base::widget(p)
        {
            p = inholder(this); size_in_bytes = sizeof(T);
            if (p && parent && p != parent) throw std::runtime_error("widget: wrong parent");
            if (p && !parent) { parent = p; parent->children += this; }
            if (!p) widgets.emplace(this);
        }
        ~widget () {
            auto it = widgets.lower_bound(this);
            if (it != widgets.end() && *it == this) widgets.erase(it);
        }
    };
}
