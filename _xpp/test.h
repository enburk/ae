#pragma once
#include "gui_widget_image.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "test_fonts.h"

using pix::XY;
using pix::XYWH;
using pix::XYXY;
using pix::RGBA;
using pix::image;
using pix::frame;

struct Test : gui::widget<Test>
{
    gui::canvas canvas;
    gui::widgetarium<gui::button> buttons;
    array<gui::base::widget*> tests;

    TestFont1 test_font1;
    TestFont2 test_font2;
    TestFont3 test_font3;

    Test ()
    {
        canvas.color = pix::red;

        tests += &test_font1; buttons.emplace_back().text.text = "Fonts1";
        tests += &test_font2; buttons.emplace_back().text.text = "Fonts2";
        tests += &test_font3; buttons.emplace_back().text.text = "Fonts3";

        for (auto & button : buttons) button.kind = gui::button::sticky;

        for (auto & test : tests) test->hide();
    }

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; int w = gui::metrics::text::height*10;
            int H = coord.now.h; int h = gui::metrics::text::height*2;

            canvas.coord = coord.now.local(); int y = 0;

            buttons.coord = XYWH(W-w, 0, w, H);

            for (auto & button : buttons) { button.coord = XYWH(0, y, w, h); y += h; }

            for (auto & test : tests) test->coord = XYWH(0, 0, W-w, H);
        }
    }

    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &buttons) {
            if (!buttons(n).on) return;
            for (int i=0; i<buttons.size(); i++) {
                buttons(i).on = i == n;
                tests[i]->show (i == n,
                    gui::time(500));
            }
        }
    }
};