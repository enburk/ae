#pragma once
#include <set>
#include "pix.h"
#include "aux_utils.h"
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

        virtual void on_render (Frame<RGBA> frame, XY offset, uint8_t alpha) {}
        virtual void on_change (void* what) { on_change(); }
        virtual void on_change () {}

        enum Opacity { transparent, semitransparent, opaque };
        virtual Opacity opacity () { return transparent; }

        Rectifier updates;
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

        void render (Frame<RGBA> frame, XY offset, uint8_t combined_alpha = 255)
        {
            combined_alpha = ((combined_alpha+1) * alpha.now) >> 8; if (combined_alpha == 0) return;
            on_render (frame, offset, combined_alpha);
            for (auto child : children) {
                XYWH  child_global = child->coord.now + frame.offset - offset;
                auto  child_frame = frame.frame(child_global - frame.offset);
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

        virtual bool mouse_sensible (XY p) { return false; }
        virtual void on_mouse_press (XY, char button, bool down) {}
        virtual void on_mouse_wheel (XY, int) {}
        virtual void on_mouse_hover (XY) {}
        virtual void on_mouse_leave () {}

        bool mouse_sense (XY p)
        {
            if (alpha.now == 0 || (coord.now - coord.now.origin).excludes(p)) return false;
            for (auto w : children)
                if (w->mouse_sense (p - w->coord.now.origin))
                    return true;
            return mouse_sensible(p);
        }

        void mouse_press (XY p, char button, bool down)
        {
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
                }
            }
            if (mouse_hover_child &&
                mouse_hover_child != hover)
                mouse_hover_child->on_mouse_leave();

            mouse_hover_child = hover;

            if (!hover) on_mouse_hover(p);
        }

        void mouse_leave ()
        {
            if (mouse_hover_child) {
                mouse_hover_child->mouse_leave ();
                mouse_hover_child = nullptr;
            }
            on_mouse_leave ();
        }

        void mouse_wheel (XY p, int delta)
        {
        }

        ////////////////////////////////////////////////////////////////////////

        void notify () { if (parent) parent->on_notify(this); }

        virtual void on_notify (widget*) {}

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

    template<class T> struct widgetarium : widget<widgetarium<T>>
    {
        using widget = widget<widgetarium<T>>;
        using widget::children;

        array<int> holes;
        array<int> indices;
        std::deque<std::optional<T>> deque;

        int size () const { return indices.size(); }

        const T & operator () (int pos) const { return *deque[indices[pos]]; }
        /***/ T & operator () (int pos) /***/ { return *deque[indices[pos]]; }

        const T & back () const { return *deque[indices.back()]; }
        /***/ T & back () /***/ { return *deque[indices.back()]; }

        template<class... Args>
        T & emplace_back (Args&&... args)
        {
            if (holes.empty()) { auto & t =
                deque.emplace_back(std::in_place, std::forward<Args>(args)...);
                indices.emplace_back(size());
                children += &t.value();
                t.value().parent = this;
                return t.value();
            } else {
                int index = holes.back(); holes.pop_back(); auto & t =
                deque[index].emplace(std::forward<Args>(args)...);
                indices.emplace_back(index);
                children += &t;
                t.parent = this;
                return t;
            }
        }

        int rotate (int f, int m, int l) {
            assert(children.size() == indices.size());
            std::rotate(
            children.begin() + f,
            children.begin() + m,
            children.begin() + l);
            return (int)(
            std::rotate(
            indices.begin() + f,
            indices.begin() + m,
            indices.begin() + l) -
            indices.begin());
        }

        void erase (int pos)
        {
            deque[indices[pos]].reset();
            holes.push_back(indices[pos]);
            indices.erase(indices.begin()+pos);
        }
        void truncate (int pos) { while (size() > pos) erase(size()-1); }
        void clear () { truncate(0); }

        template<class U> struct iterator
        {
            widgetarium<U>* that; int index;
            void operator ++ () { ++index; }
            void operator -- () { --index; }
            void operator += (int n) { index += n; }
            void operator -= (int n) { index -= n; }
            bool operator == (iterator i) { return index == i.index; }
            bool operator != (iterator i) { return index != i.index; }
            const U & operator * () const { return (*that)(index); }
            /***/ U & operator * () /***/ { return (*that)(index); }
            friend iterator operator + (iterator i, int n) { i.index += n; return i; }
            friend iterator operator - (iterator i, int n) { i.index -= n; return i; }
            friend int operator - (iterator i, iterator j) { return i.index - j.index; }
        };
        iterator<T> begin () { return iterator<T>{this, 0}; }
        iterator<T> end   () { return iterator<T>{this, size()}; }
    };
}
