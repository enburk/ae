#pragma once
#include "sys.h"
#include "gui_widget.h"
#include <thread>
#include <atomic>

namespace gui { inline gui::base::widget * window = nullptr; }

void sys::window::on::resize() {
    gui::window->resize(sys::window::image.size);
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
    gui::window->render(sys::window::image.frame(rect), rect.origin);
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

void sys::window::on::start () { gui::timer_stop = false; gui::timer = std::thread (gui::timer_proc); }
void sys::window::on::finish() { gui::timer_stop = true; if (gui::timer.joinable()) gui::timer.join(); }
void sys::window::on::pause () { gui::timer_stop = true; }
void sys::window::on::resume() {
     sys::window::on::finish(); gui::time::set_after_pause();
     sys::window::on::timing(); gui::time::reset_after_pause();
     sys::window::on::start ();
}
