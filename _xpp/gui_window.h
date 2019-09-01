#pragma once
#include "pix.h"
#include "sys.h"
#include "gui_widget.h"

inline Widget * Window = nullptr;

void sys::mouse::on::clickclick(XY p, char button)
{
    Window->on_mouse_clickclick(p, button);
}
void sys::mouse::on::press(XY p, char button, bool down) {

    capture
}
void sys::mouse::on::move(XY p) {
    test::mouse::on::move(p);
}

void sys::window::on::resize() {
    test::window::on::resize();
}
void sys::window::on::redraw(XYXY r) {
    test::window::on::redraw(r);
}
