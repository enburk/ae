#pragma once
#include "gui_widget_image.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "test_fonts.h"

using pix::XY;
using pix::XYWH;
using pix::XYXY;
using pix::RGBA;
using pix::Image;
using pix::Frame;

struct Test : gui::widget<Test>
{
    gui::canvas canvas;
    gui::button button_font1; TestFont1 test_font1;
    gui::button button_font2; TestFont2 test_font2;
    gui::button button_font3; TestFont3 test_font3;

    Test ()
    {
        canvas.color = pix::red;
        test_font1.hide();
        test_font2.hide();
        test_font3.hide();
    }

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            int W = coord.now.w; int w = W/30; int dx = w/10;
            int H = coord.now.h; int h = H/40; int dy = h/10;

            canvas.coord = coord.now.local();  int y = dy;

            button_font1.coord = XYWH(W-w-dx, y, w, h); y += h + dy;
            button_font2.coord = XYWH(W-w-dx, y, w, h); y += h + dy;
            button_font3.coord = XYWH(W-w-dx, y, w, h); y += h + dy;

            test_font1.coord = XYWH(0, 0, W-dx-w-dx, H);
            test_font2.coord = XYWH(0, 0, W-dx-w-dx, H);
            test_font3.coord = XYWH(0, 0, W-dx-w-dx, H);
        }
    }

    virtual void on_notify (gui::base::widget* w)
    {
        test_font1.show (w == &button_font1, gui::time(500));
        test_font2.show (w == &button_font2, gui::time(500));
        test_font3.show (w == &button_font3, gui::time(500));
    }
};