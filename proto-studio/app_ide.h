#pragma once
#include "data_ranges.h"
#include "gui_widget.h"
#include "gui_widget_canvas.h"
#include "sys_aux.h"
#include "sys_ui.h"
#include "app_ide_console.h"
#include "app_ide_editor.h"
#include "app_ide_flist.h"
#include "app_ide_test.h"
using namespace aux;
using namespace pix;

struct IDE : gui::widget<IDE>
{
    gui::canvas canvas;

    gui::canvas toolbar;
    gui::button button_run;
    gui::button button_test;

    gui::area<Flist> flist_area;
    gui::area<Editor> editor_area;
    gui::area<Console> console_area;
    gui::splitter splitter_editor_l;
    gui::splitter splitter_editor_r;

    Flist& flist = flist_area.object;
    Editor& editor = editor_area.object;
    Console& console = console_area.object;
    gui::area<Test> test_area; // very last

    IDE()
    {
        skin = "gray";
        gui::skins[skin].font =
        sys::font{"Segoe UI", gui::metrics::text::height};
        toolbar.color = gui::skins[skin].light.first;
        button_run .text.text = "run";
        button_test.text.text = "test";
        test_area.hide();
        canvas.color = RGBA::red;
    }

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*12/7;
            
            int d = gui::metrics::line::width * 6;
            int l = sys::settings::load("splitter.editor.l.permyriad", 18'00) * W / 100'00;
            int r = sys::settings::load("splitter.editor.r.permyriad", 70'00) * W / 100'00;

            splitter_editor_l.coord = XYXY(l-d, h, l+d, H);
            splitter_editor_r.coord = XYXY(r-d, h, r+d, H);
            splitter_editor_l.lower = 10'00 * W / 100'00;
            splitter_editor_l.upper = 35'00 * W / 100'00;
            splitter_editor_r.lower = 65'00 * W / 100'00;
            splitter_editor_r.upper = 90'00 * W / 100'00;

            toolbar.coord = XYWH(0, 0, W, h);
            button_run .coord = XYWH(0, 0, w, h);
            button_test.coord = XYWH(W-w, 0, w, h);

            flist_area.coord = XYWH(0, h, l-0, H-h);
            editor_area.coord = XYWH(l, h, r-l, H-h);
            console_area.coord = XYWH(r, h, W-r, H-h);

            if (test_area.alpha.now != 0)
                test_area.coord = XYXY(0, h, W, H);

            canvas.coord = coord.now.local();
        }
    }

    void on_notify (void* what) override
    {
        if (what == &splitter_editor_l) {
            sys::settings::save(
                "splitter.editor.l.permyriad",
                 splitter_editor_l.middle
                * 100'00 / coord.now.w);
            on_change(&coord);
        }
        if (what == &splitter_editor_r) {
            sys::settings::save(
                "splitter.editor.r.permyriad",
                 splitter_editor_r.middle
                * 100'00 / coord.now.w);
            on_change(&coord);
        }
        if (what == &button_test)
        {
            bool on = test_area.alpha.to != 0;
            bool turn_on = not on;
            test_area.show(turn_on);
            test_area.coord = turn_on ?
                XYXY(0, toolbar.coord.now.h, coord.now.w, coord.now.h):
                XYXY{};
        }
    }
};
sys::app<IDE> app("AE proto-studio");//, {0,0}, {100, 100});



