#pragma once
#include "data_struct_array.h"
#include "data_struct_string.h"
#include "gui_widget_canvas.h"
#include "gui_widget_console.h"
using namespace pix;

struct TestFirst : gui::widget<TestFirst>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return; int w = W/2;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = XYWH(0, 0, w, H);
            console2.coord = XYWH(w, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
                sys::font{"Consolas", gui::metrics::text::height},
                RGBA::black};
            console1.object.page.style = style;
            console2.object.page.style = style;

            data::unittest::test_array();
            data::unittest::test("");
            console1.object.page.html = 
            data::unittest::results; ok &= 
            data::unittest::all_ok;

            data::unittest::test_string();
            data::unittest::test("");
            console2.object.page.html = 
            data::unittest::results; ok &= 
            data::unittest::all_ok;

            console1.object.page.scroll.y.top = max<int>();
            console2.object.page.scroll.y.top = max<int>();
        }
    }
};

struct TestFonts : gui::widget<TestFonts>
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

struct TestTexts : gui::widget<TestTexts>
{
    gui::button doubling;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::splitter splitter; int x = 40'00; int xx = 80;
    TestTexts () { on_notify(&doubling); }
    str text = "<b>Lorem ipsum</b> dolor sit amet, consectetur adipiscing <i>elit</i>, "
    "sed do eiusmod tempor incididunt ut labore et dolore <b>magna <i>aliqua.</i></b> "
    "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
    "<font color=#008000><i>ex ea commodo</i></font> consequat. Duis aute irure dolor "
    "in reprehenderit <b><i><u>in</u></i></b> voluptate <font color=#000080>velit</font> "
    "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
    "proident, sunt in culpa qui officia deserunt mollit anim id est "
    "<b><font color=#008000>laborum.</font></b><br><br>";
    void on_change (void* what) override
    {
        if (what == &skin)
        {
            console1.object.page.view.canvas.color = RGBA::white;
            console2.object.page.view.canvas.color = RGBA::white;
        }
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*xx;
            int h = gui::metrics::text::height*12/7;
            int d = gui::metrics::line::width * 6;
            int m = w * x / 100'00;
            doubling.coord = XYWH(w-0, 0, w/9, h);
            console1.coord = XYXY(0-0, 0, m+0, H);
            console2.coord = XYXY(m-0, 0, w+0, H);
            splitter.coord = XYXY(m-d, 0, m+d, H);
            splitter.lower = 20'00 * w / 100'00;
            splitter.upper = 80'00 * w / 100'00;
        }
    }
    void on_notify (void* what) override
    {
        if (what == &doubling)
        {
            doubling.text.text = "double text";
            console1.object.page.html = text;
            console2.object.page.html = text;
            text = text + text;
        }
        if (what == &splitter)
        {
            int w = gui::metrics::text::height*xx;
            x = 100'00 * splitter.middle / w;
            on_change(&coord);
        }
    }
};

struct Test : gui::widget<Test>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    array<gui::base::widget*> tests;
    TestFirst test_first;
    TestFonts test_fonts;
    TestTexts test_texts;

    Test ()
    {
        tests += &test_first; buttons.emplace_back().text.text = "unit test";
        tests += &test_fonts; buttons.emplace_back().text.text = "test fonts";
        tests += &test_texts; buttons.emplace_back().text.text = "test texts";

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
