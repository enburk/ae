#pragma once
#include "../sys.h"
#include "gui_colors.h"
#include "gui_widget.h"
#include <thread>
#include <atomic>

namespace gui { inline gui::base::widget * window = nullptr; }

void sys::window::on::resize() {
     gui::window->resize(sys::window::image.size);
     sys::window::on::timing();
}
void sys::keyboard::on::focus(bool on) {
     gui::window->on_focus(on);
     sys::window::on::timing();
}
void sys::keyboard::on::press(str key, bool down) {
     gui::window->on_key_pressed(key, down);
     sys::window::on::timing();
}
void sys::keyboard::on::input(str symbol) { if (symbol != "")
     gui::window->on_keyboard_input(symbol);
     sys::window::on::timing();
}
void sys::mouse::on::press(XY p, char button, bool down) {
     gui::window->mouse_press(p, button, down);
     sys::window::on::timing();
}
void sys::mouse::on::wheel(XY p, int delta) {
     gui::window->mouse_wheel(p, delta);
     sys::window::on::timing();
}
void sys::mouse::on::move(XY p) {
     gui::window->mouse_move(p);
     sys::window::on::timing();
}
void sys::mouse::on::leave() {
     gui::window->mouse_leave();
     sys::window::on::timing();
}

void sys::window::on::timing()
{
     gui::time::set();
     gui::active_properties.for_each( [](auto p){ p->tick(); } );
     for (auto rect : gui::window->updates)
     gui::window->render(sys::window::image.crop(rect), rect.origin);
     gui::window->updates.clear();
}

namespace gui
{
    std::atomic <bool> timer_stop = true;
    inline std::thread timer;
    auto timer_proc = []()
    {
        while (true) {
            if (timer_stop) break; std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));
            if (timer_stop) break; sys::window::timing();
        }
    };
}

void sys::app_instance::done  () {}
void sys::app_instance::init  () { gui::init(); }
void sys::window::on::finish  () { sys::window::on::turn_off(); gui::window->on_close(); }
void sys::window::on::start   () { sys::window::on::turn_on (); }
void sys::window::on::turn_on () { gui::timer_stop = false; gui::timer = std::thread (gui::timer_proc); }
void sys::window::on::turn_off() { gui::timer_stop = true; if (gui::timer.joinable()) gui::timer.join(); }
void sys::window::on::pause   () { gui::timer_stop = true; }
void sys::window::on::resume  () {
     sys::window::on::turn_off(); gui::time::set_after_pause();
     sys::window::on::timing  (); gui::time::reset_after_pause();
     sys::window::on::turn_on ();
}
