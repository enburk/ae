#pragma once
#include "pix.h"

struct Flist
{
    str filename;

    FRAME<XRGB> frame;

    void on_resize()
    {
    }
    void on_mouse_clickclick(XY p, char button) {
    }
    void on_mouse_press(XY p, char button, bool down) {
    }
    void on_mouse_move(XY p) {
    }
}; 