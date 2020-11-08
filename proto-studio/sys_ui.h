#pragma once
#include <thread>
#include <atomic>
#include "aux_string.h"
#include "aux_utils.h"
#include "pix_color.h"
#include "pix_image.h"
#include "gui_colors.h"
#include "gui_effect.h"
namespace sys
{
    using namespace aux;
    using namespace pix;

    namespace screen
    {
        inline XY size;
    }
    namespace keyboard
    {
        inline bool alt = false;
        inline bool ctrl = false;
        inline bool shift = false;
    }
    namespace mouse
    {
        void cursor(str image);
    }

    enum class choice
    {
        ok,               
        ok_cancel,
        yes_no,
        yes_no_cancel,
        abort_retry_ignore,
        cancel_try_continue,
        retry_cancel
    };
    str dialog (str title, str text, choice, void* handle = nullptr);

    struct frame
    {
        XY offset, size;
        explicit operator XYWH () const { return
            XYWH (offset.x, offset.y, size.x, size.y); }

        frame crop (XYWH r) const {
            r = XYWH(*this) & (r + offset);
            frame f = *this;
            f.offset = XY(r.x, r.y);
            f.size   = XY(r.w, r.h);
            return f;
        }
        std::function<void(frame, RGBA, uint8_t alpha)> blend;
    };

    struct window : polymorphic
    {
        pix::image<RGBA> image;
        void*native_handle1 = nullptr;
        void*native_handle2 = nullptr;
        bool opengl = false;

        void create (str title);
        void update ();
        void timing ();

        str dialog (str title, str text, choice choice) {
            return sys::dialog(title, text, choice,
                native_handle1); }

        virtual void on_timing () = 0;
        virtual void on_resize (XY size) = 0;
        virtual void keyboard_on_focus (bool on) = 0;
        virtual void keyboard_on_press (str key, bool down) = 0;
        virtual void keyboard_on_input (str symbol) = 0;
        virtual void mouse_on_press (XY p, char button, bool down) = 0;
        virtual void mouse_on_wheel (XY p, int delta) = 0;
        virtual void mouse_on_move  (XY p) = 0;
        virtual void mouse_on_leave () = 0;
        virtual void render (frame) {}

        std::thread timer;
        std::atomic<bool> timer_stop = true;
        std::function<void(void)> timer_proc = [this]()
        {
            while (true) {
                using namespace std::literals::chrono_literals;
                if (timer_stop) break; std::this_thread::sleep_for(16ms); // 60 FPS
                if (timer_stop) break; timing();
            }
        };

        void on_finish  () { on_turn_off(); }
        void on_start   () { on_turn_on (); }
        void on_turn_on () { timer_stop = false; timer = std::thread (timer_proc); }
        void on_turn_off() { timer_stop = true; if (timer.joinable()) timer.join(); }
        void on_pause   () { timer_stop = true; }
        void on_resume  () {
             on_turn_off(); gui::time::set_after_pause();
             on_timing  (); gui::time::reset_after_pause();
             on_turn_on ();
        }
    };

    template<class Widget> struct pixwindow : window
    {
        Widget widget;

        void keyboard_on_focus(bool on) override {
             widget.on_focus(on);
             on_timing();
        }
        void keyboard_on_press(str key, bool down) override {
             widget.on_key_pressed(key, down);
             on_timing();
        }
        void keyboard_on_input(str symbol) override { if (symbol != "")
             widget.on_keyboard_input(symbol);
             on_timing();
        }
        void mouse_on_press(XY p, char button, bool down) override {
             widget.mouse_press(p, button, down);
             on_timing();
        }
        void mouse_on_wheel(XY p, int delta) override {
             widget.mouse_wheel(p, delta);
             on_timing();
        }
        void mouse_on_move(XY p) override {
             widget.mouse_move(p);
             on_timing();
        }
        void mouse_on_leave() override {
             widget.mouse_leave();
             on_timing();
        }
        void on_resize (XY size) override {
             image .resize(size);
             widget.resize(size);
             on_timing();
        }
        void on_timing() override
        {
             gui::time::set();
             gui::active_properties.for_each([](auto p){ p->tick(); });
             for (auto rect : widget.updates)
             {
                  frame f {rect.origin, rect.size};
                  f.blend = [this](frame f, RGBA c, uint8_t alpha)
                  { image.crop(XYWH(f)).blend(c, alpha); };
                  widget.render(f, rect.origin);
             }
             widget.updates.clear();
        }
    };

    template<class Widget> struct glxwindow : pixwindow<Widget>
    {
        using pixwindow<Widget>::opengl;
        using pixwindow<Widget>::widget;
        using pixwindow<Widget>::update;
        using pixwindow<Widget>::image;

        glxwindow() { opengl = true; }

        void on_resize (XY size) override {
             widget.resize(size);
             widget.update();
             on_timing();
        }
        void on_timing() override
        {
             gui::time::set();
             gui::active_properties.for_each([](auto p){ p->tick(); });
             image.updates = widget.updates;
             widget.updates.clear();
             update();
        }
        void render (frame f) override
        {
            widget.render(f, f.offset);
        }
    };

    struct app_base : polymorphic
    {
        str title;
        virtual void constructor() = 0;
        virtual void destructor () = 0;
    };
    struct app_instance
    {
        static void init() { gui::init(); }
        static void done() { gui::done(); }
        static inline app_base * app = nullptr;
        app_instance () { init(); if (app) app->constructor(); }
       ~app_instance () { if (app) app->destructor();  done(); }
    };
    template<class Widget> struct app : app_base
    {
        glxwindow<Widget> * winptr = nullptr;
        app (str s) { app_instance::app = this; title = s; }
        void destructor () override { delete winptr; }
        void constructor() override { winptr = new
            typename std::remove_reference
            <decltype(*winptr)>::type{};
            winptr->create(title);
        }
    };
}