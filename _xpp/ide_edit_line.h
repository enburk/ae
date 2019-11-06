#pragma once
#include "doc.h"
#include "pix.h"

struct Widget
{
    bool mouse_pressed;
    array<str> key_pressed;

    virtual void on_resize()
    {

    }
    virtual void on_mouse_clickclick(XY p, char button)
    {
    }
    virtual void on_mouse_press(XY p, char button, bool down)
    {
    }
    virtual void on_mouse_move(XY p)
    {
    }
};

struct Settings
{
    Font font;
    XRGB fore;
    XRGB back;
    bool read_only;
    bool virtual_space;
};

struct Options
{
    Font font;
    XRGB fore;
    XRGB back;
    bool read_only;
    bool virtual_space;
};

struct Line : Widget
{
    frame<XRGB> frame;


    using Glyph = Glyph<XRGB>;

    array<Glyph> glyphs;

    Settings settings;

    Options opt;

    int baseline;
    int caret;

    void insert (Range<Token> & tokens)
    {
        if (opt.read_only) return;
    }



    void on_resize() override
    {

    }
    void on_mouse_clickclick(XY p, char button) override
    {
    }
    void on_mouse_press(XY p, char button, bool down) override
    {
        if (settings.virtual_space) {}
    }
    void on_mouse_move(XY p) override
    {
        if (opt.virtual_space) {}
    }
};
