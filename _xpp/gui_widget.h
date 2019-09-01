#pragma once
#include "pix.h"

struct Widget
{
    static Widget* MousePressWidget;
    static Widget* MouseHoverWidget;

    array<Widget*> children;
    Widget* parent = nullptr;
    Widget* owner = nullptr;
    FRAME<XRGB> frame;
    int id = -1;

    Widget (Widget * parent) : parent(parent), owner(parent) {
        if (parent) parent->children += this;
        if (parent) frame = parent->frame;
    }

    virtual void on_mouse_clickclick (XY p, char button) {
        for (auto w : children)
            if (XYWH(w->frame).include(p - w->frame.origin)) {
                w->on_mouse_clickclick(p - w->frame.origin, button);
                return; }
    }

    virtual void on_mouse_press (XY p, char button, bool down) {
        for (auto w : children)
            if (XYWH(w->frame).include(p - w->frame.origin)) {
                w->on_mouse_press     (p - w->frame.origin, button, down);
                return; }
        if (button == 'L') MousePressWidget = down ? this : nullptr;
    }

    virtual void on_mouse_move (XY p) {
        for (auto w : children)
            if (XYWH(w->frame).include(p - w->frame.origin)) {
                w->on_mouse_press     (p - w->frame.origin, button, down);
                return; }
        if (button == 'L') MousePressWidget = down ? this : nullptr;
    }
};

void sys::mouse::on::press(XY p, char button, bool down) {

    capture
}
void sys::mouse::on::move(XY p) {
    test::mouse::on::move(p);
}
