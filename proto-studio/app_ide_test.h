#pragma once
#include "gui_widget_canvas.h"
using namespace pix;

struct TestFirst : gui::widget<TestFirst>
{
    gui::canvas canvas;
    void on_change (void* what) override
    {
        if (what == &skin) canvas.color = RGBA::white;
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            canvas.coord = XYWH(0, 0, W, H);
        }
    }
};

struct TestFont1 : gui::widget<TestFont1>
{
    gui::image gui_image;
    pix::image<RGBA> image;

    void on_change (void* what) override
    {
        if (what != &alpha || alpha.to == 0 ||
            coord.now.size == image.size ||
            coord.now.size == XY()) return;

        image.resize(coord.now.size); image.fill(RGBA::red);
        gui_image.coord = coord.now.local();
        gui_image.source = image;

        str digit = "0123456789";
        str Latin = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        str latin = "abcdefghijklmnopqrstuvwxyz";
        str alnum = Latin + latin + digit;
        array<
        sys::font> fonts = {
        sys::font{"Consolas", gui::metrics::text::height*5/2},
        sys::font{"Arial",    gui::metrics::text::height*5/2},
        sys::font{"Segoe UI", gui::metrics::text::height*5/2},
        sys::font{"Tahoma",   gui::metrics::text::height*5/2}};

        int gap = 1; int x = gap; int y = gap;

        for (auto font : fonts)
        for (int r=0; r<4; r++, x = gap, y += gap +
            sys::metrics(font).ascent  +
            sys::metrics(font).descent +
            sys::metrics(font).linegap)
        {
            pix::text::style style;
            style.color = RGBA::black;
            style.color.a = 230;
            style.font = font;
            style.font.bold   = r == 2 || r == 3;
            style.font.italic = r == 1 || r == 3;

            for (char c : alnum) {
                auto glyph = sys::glyph(str(c), style);
                auto w = glyph.width;
                auto h = glyph.ascent+glyph.descent;
                auto frame = image.crop(XYWH(x, y, w, h));
                frame.blend(RGBA::white, 200);
                glyph.render(frame, XY(), 200, x);
                x += glyph.width + glyph.advance + gap;
            }
        }
    }
};

struct Test : gui::widget<Test>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    array<gui::base::widget*> tests;

    TestFirst test_first;
    TestFont1 test_font1;

    Test ()
    {
        tests += &test_first; buttons.emplace_back().text.text = "unit test";
        tests += &test_font1; buttons.emplace_back().text.text = "test fonts 1";

        buttons(0).on = true;
        for (int i=1; i<tests.size(); i++)
            tests[i]->hide();
    }

    void on_change (void* what) override
    {
        if (what == &skin) canvas.color = RGBA::red;
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*12/7;
            int y = 0;

            canvas.coord = XYWH(0, 0, W, H);
            buttons.coord = XYWH(W-w, 0, w, H);

            for (auto & button : buttons) { button.coord = XYWH(0, y, w, h); y += h; }

            for (auto & test : tests) test->coord = XYWH(0, 0, W-w, H);
        }
    }

    void on_notify (void* what) override
    {
        if (what == &buttons)
            for (int i=0; i<tests.size(); i++)
                tests[i]->show(buttons(i).on.now);
    }
};
