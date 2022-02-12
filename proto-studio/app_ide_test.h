#pragma once
#include "../../auxs/test_aux.h"
#include "../../auxs/test_doc.h"
#include "../../auxs/test_gui.h"
#include "../../auxs/test_pix.h"

struct Test:
widget<Test>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    array<gui::base::widget*> tests;
    TestFirst test_first;
    TestFirst test_model;
    TestFonts test_fonts;
    TestTexts test_texts;
    TestCoros test_coros;
    TestMonad test_monad;
    TestColor test_color;
    TestWideo test_wideo;

    Test ()
    {
        tests += &test_first; buttons.emplace_back().text.text = "unit test";
        tests += &test_model; buttons.emplace_back().text.text = "text model";
        tests += &test_fonts; buttons.emplace_back().text.text = "test fonts";
        tests += &test_texts; buttons.emplace_back().text.text = "test texts";
        tests += &test_coros; buttons.emplace_back().text.text = "coroutines";
        tests += &test_monad; buttons.emplace_back().text.text = "monadic";
        tests += &test_wideo; buttons.emplace_back().text.text = "widgets";
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

            for (auto & button : buttons) {
                button.coord = XYWH(0, y, w, h);
                y += h; }

            for (auto & test : tests)
                test->coord = XYWH(0, 0, W-w, H);
        }

        if (what == &buttons)
            for (int i=0; i<tests.size(); i++)
                tests[i]->show(buttons(i).on.now);
    }
};
