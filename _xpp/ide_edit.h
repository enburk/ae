#pragma once
#include "doc.h"
#include "pix.h"

struct Editor
{
    str filename;

    FRAME<XRGB> frame;

    FONT font;
    XRGB fore;
    XRGB back;

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
