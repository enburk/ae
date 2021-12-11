#pragma once
#include "aux_abc.h"
#include "aux_monad.h"
#include "aux_unittest_coro.h"
#include "aux_unittest_array.h"
#include "aux_unittest_string.h"
#include "gui_widget_canvas.h"
#include "gui_widget_console.h"
#include "doc_text_model_a.h"
#include "doc_text_model_b.h"
using namespace pix;
using gui::widget;

struct TestFirst:
widget<TestFirst>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::area<gui::console> console3;
    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return; int w = W/3;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = XYWH(w*0, 0, w, H);
            console2.coord = XYWH(w*1, 0, w, H);
            console3.coord = XYWH(w*2, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
            sys::font{"Consolas"}, RGBA::black};
            console1.object.page.style = style;
            console2.object.page.style = style;
            console3.object.page.style = style;

            aux::unittest::test_array();
            aux::unittest::test("");
            console1.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_string();
            aux::unittest::test("");
            console2.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::text_model_a();
            aux::unittest::text_model_b();
            aux::unittest::test("");
            console3.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console1.object.page.scroll.y.top = max<int>();
            console2.object.page.scroll.y.top = max<int>();
            console3.object.page.scroll.y.top = max<int>();
        }
    }
};

struct TestFonts:
widget<TestFonts>
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

struct TestTexts:
widget<TestTexts>
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

struct TestCoros:
widget<TestCoros>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::button asyncs;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::area<gui::console> console3;
    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int h = gui::metrics::text::height*12/7;
            int W = coord.now.w; if (W <= 0) return; int w = (W-5*h)/3;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = XYWH(w*0, 0, w, H);
            console2.coord = XYWH(w*1, 0, w, H);
            console3.coord = XYWH(w*2, 0, w, H);
            asyncs  .coord = XYWH(w*3, 0, 5*h, h);
            asyncs.text.text = "asyncs";

            if (done) return; done = true;

            auto style = pix::text::style{
            sys::font{"Consolas"}, RGBA::black};
            console1.object.page.style = style;
            console2.object.page.style = style;
            console3.object.page.style = style;

            aux::unittest::test_coro1();
            aux::unittest::test("");
            console1.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_coro2();
            aux::unittest::test("");
            console2.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console1.object.page.scroll.y.top = max<int>();
            console2.object.page.scroll.y.top = max<int>();
            console3.object.page.html = "press a button "
            "------------------------------------------>";
        }
    }
    void on_notify (void* what) override
    {
        if (what == &asyncs)
        {
            aux::unittest::test_coro3();
            aux::unittest::test("");
            console3.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console3.object.page.scroll.y.top = max<int>();
        }
    }
};

struct TestMonad:
widget<TestMonad>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::area<gui::console> console3;
    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int h = gui::metrics::text::height*12/7;
            int W = coord.now.w; if (W <= 0) return; int w = W/3;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = XYWH(w*0, 0, w, H);
            console2.coord = XYWH(w*1, 0, w, H);
            console3.coord = XYWH(w*2, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
            sys::font{"Consolas"}, RGBA::black};
            console1.object.page.style = style;
            console2.object.page.style = style;
            console3.object.page.style = style;

            aux::unittest::test_monad1();
            aux::unittest::test("");
            console1.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_monad2();
            aux::unittest::test("");
            console2.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_monad3();
            aux::unittest::test("");
            console3.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console1.object.page.scroll.y.top = max<int>();
            console2.object.page.scroll.y.top = max<int>();
            console3.object.page.scroll.y.top = max<int>();
        }
    }
};

struct TestColor:
widget<TestColor>
{
    struct sample:
    widget<sample>
    {
        str name;
        gui::schema schema;
        std::pair<RGBA,RGBA> colors[10];
        gui::text::view palette[10];
        gui::text::view title;
        gui::text::view error1;
        gui::text::view error2;
        gui::canvas toolbar;
        gui::radio::group buttons;
        gui::area<gui::text::page> page;
        str lorem = "<b>Lorem ipsum</b><br>"
        "Lorem ipsum dolor sit amet, <font color=#008000>consectetur</font>"
        "<font color=#000080>adipiscing</font> <i>elit</i>, sed do eiusmod tempor incididunt"
        "<font color=#800000>ut labore et dolore</font> <b>magna <i>aliqua.</i></b> "
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
        "<font color=#008000><i>ex ea commodo</i></font> consequat. Duis aute irure dolor "
        "in reprehenderit <b><i><u>in</u></i></b> voluptate <font color=#000080>velit</font> "
        "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
        "proident, sunt in culpa qui officia deserunt mollit anim id est "
        "<b><font color=#008000>laborum.</font></b><br><br>";

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                int h = gui::metrics::text::height*12/7;
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int q = W/10;

                title.coord = XYWH(0,0, W,h);
                title.color = RGBA::white;
                title.html = "<b>"+name+"</b>";

                for (int i=0; i<10; i++) {
                    palette[i].coord = XYWH(q*i, h, q, q);
                    palette[i].canvas.color = colors[i].first;
                    palette[i].color = colors[i].second;
                    palette[i].text = std::to_string(i);
                }

                buttons.coord = XYWH(0,h+q, W,h);
                toolbar.coord = XYWH(0,h+q, W,h);
                toolbar.color = schema.light.first;

                int n = 3;
                for (int i=0; i<n; i++) {
                    buttons(i).text.text = "button";
                    buttons(i).coord = XYWH(i*W/n, 0, W/n, h);
                    buttons(i).skin = name;
                    buttons(i).on = i == 0;
                }

                error1.coord = XYWH(0,   h+q+h, W/2, h);
                error2.coord = XYWH(W/2, h+q+h, W/2, h);
                error1.canvas.color = schema.error.first;
                error2.canvas.color = schema.error.second;
                error1.color = schema.error.second;
                error2.color = schema.error.first;
                error1.text = "error";
                error2.text = "error";

                page.skin = name;
                page.coord = XYXY(0,h+q+h+h, W, H);
                page.object.html = lorem;
                page.object.alignment = XY{pix::left, pix::top};
                page.object.view.canvas.color = schema.ultralight.first;
                page.object.style = pix::text::style{
                    sys::font{"Segoe UI", h*4/7},
                    schema.dark.first};
            }
        }
    };

    gui::widgetarium<sample> samples;

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;

            samples.coord = coord.now.local();

            if (samples.size() == 0)
            {
                for (auto [i, palette]: enumerate(gui::palettes)) {
                    auto [name, colors] = palette;
                    for (int j=0; j<10; j++)
                    samples(i).colors[j] = colors[j];
                    samples(i).schema = gui::skins[name];
                    samples(i).name = name == "" ?
                        "random" : name;
                }
            }

            int n = samples.size();
            int square = W*H/n;
            int side = (int)(std::sqrt(square));
            if (side < 10) return;
            int nx = W/side;
            int ny = H/side;
            while (nx*ny < n) {
                side--; if (side < 10) return;
                nx = W/side;
                ny = H/side;
            }

            for (int y=0; y<ny; y++)
            for (int x=0; x<nx; x++)
            {
                int i = y*nx + x;
                if (i >= n) break;
                samples(i).coord = XYWH(
                x*side, y*side, side, side);
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
    TestFonts test_fonts;
    TestTexts test_texts;
    TestCoros test_coros;
    TestMonad test_monad;
    TestColor test_color;

    Test ()
    {
        tests += &test_first; buttons.emplace_back().text.text = "unit test";
        tests += &test_fonts; buttons.emplace_back().text.text = "test fonts";
        tests += &test_texts; buttons.emplace_back().text.text = "test texts";
        tests += &test_coros; buttons.emplace_back().text.text = "coroutines";
        tests += &test_monad; buttons.emplace_back().text.text = "monadic";
        tests += &test_color; buttons.emplace_back().text.text = "colors";

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
