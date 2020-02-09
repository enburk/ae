#pragma once
#include "gui_window.h"
#include "gui_widget_image.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "gui_widget_splitter.h"
#include "ide_console.h"
#include "ide_flist.h"
#include "ide_editor.h"
#include "test.h"
using namespace pix;

struct IDE : gui::widget<IDE>
{
    gui::canvas toolbar;
    gui::button button_test;

    Console console;
    Editor editor;
    Flist flist;
    Test test;

    gui::splitter splitter_editor_l;
    gui::splitter splitter_editor_r;

    struct document
    {
        doc::Document document;
        //gui::text::editor::model::context context;
    };

    std::map<std::filesystem::path, document> documents;

    IDE()
    {
        gui::window = this;
        button_test.text.text = "Test";
        button_test.kind = gui::button::toggle;
        test.hide();
    }

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            gui::schemas[""].font = sys::font{"Segoe UI", gui::metrics::text::height};

            toolbar.color = gui::schemas[""].light.back_color;

            int W = coord.now.w; int w = gui::metrics::text::height*10;
            int H = coord.now.h; int h = gui::metrics::text::height*2;

            toolbar.coord = XYWH(0, 0, W, h);
            button_test.coord = XYWH(W-w, 0, w, h);

            int d = gui::metrics::line::width * 4;
            int l = sys::settings::load("splitter.editor.l.permyriad", 2'500) * W / 10'000;
            int r = sys::settings::load("splitter.editor.r.permyriad", 7'500) * W / 10'000;

            splitter_editor_l.coord = XYWH(l-d, h, 2*d, H-h);
            splitter_editor_r.coord = XYWH(r-d, h, 2*d, H-h);
            splitter_editor_l.lower = 1'000 * W / 10'000;
            splitter_editor_l.upper = 3'500 * W / 10'000;
            splitter_editor_r.lower = 6'500 * W / 10'000;
            splitter_editor_r.upper = 9'000 * W / 10'000;

            test.coord = XYWH(0, h, W-0, H-h);
            flist.coord = XYWH(0, h, l-0, H-h);
            editor.coord = XYWH(l, h, r-l, H-h);
            console.coord = XYWH(r, h, W-r, H-h);
        }
    }

    void on_notify (gui::base::widget* w) override
    {
        if (w == &button_test) test.show (test.alpha.to == 0, gui::time(500));

        if (w == &flist)
        {
            //documents[flist.path.was].context= editor.model.context;
            //editor.load(flist.path);
        }
    }

    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &splitter_editor_l) {
            sys::settings::save("splitter.editor.l.permyriad",
            n * 10'000 / coord.now.w);
            on_change(&coord);
        }
        if (w == &splitter_editor_r) {
            sys::settings::save("splitter.editor.r.permyriad",
            n * 10'000 / coord.now.w);
            on_change(&coord);
        }
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
};

//IDE ide;



