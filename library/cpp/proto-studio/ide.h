#pragma once
#include <experimental/coroutine>
#include "gui_window.h"
#include "gui_widget_image.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "gui_widget_splitter.h"
#include "doc_ae_translator.h"
#include "ide_console.h"
#include "ide_flist.h"
#include "ide_editor.h"
#include "test.h"
using namespace pix;

struct IDE : gui::widget<IDE>
{
    gui::canvas toolbar;
    gui::button button_save;
    gui::button button_run;
    gui::button button_test;

    gui::area<Console> console;
    gui::area<Editor> editor;
    gui::area<Flist> flist;
    gui::area<Test> test;

    gui::splitter splitter_editor_l;
    gui::splitter splitter_editor_r;

    enum class status { loading, ready, error };
    std::atomic<status> library_status = status::loading;
    gui::property<gui::time> timer;
    std::thread thread;

    IDE()
    {
        gui::window = this; skin = "gray";

        button_save.text.text = "Save";
        button_run .text.text = "Run";
        button_test.text.text = "Test";
        button_save.enabled = false;
        button_run .enabled = false;
        button_test.kind = gui::button::toggle;
        test.hide();

        thread = std::thread([this]()
        {
            console.object.console << "Prepare library...";

            for (std::filesystem::directory_iterator next
                (std::filesystem::current_path() / "library"),
                end; next != end; ++next)
            {
                auto path = next->path();
                if (std::filesystem::is_regular_file (path) &&
                    path.extension() == ".ae")
                {
                    console.object.console << path.string();
                    doc::ae::translator::add(path);
                    //if (doc::errors.size() > 0) break;
                }
            }

            if (true)//doc::errors.size() == 0)
            {
                console.object.console << "DONE.";
                library_status = status::ready;
            }
            else
            {
                library_status = status::error;
            }
        });
    }
    ~IDE()
    {
        if (thread.joinable())
            thread.join();
    }

    void on_change (void* what) override
    {
        if (timer.now == gui::time())
            timer.go (gui::time::infinity,
                      gui::time::infinity);

        if (what == &coord)
        {
            gui::skins[""].font = sys::font{"Segoe UI", gui::metrics::text::height};

            toolbar.color = gui::skins[""].light.back_color;

            int W = coord.now.w; int w = gui::metrics::text::height*10;
            int H = coord.now.h; int h = gui::metrics::text::height*12/7;
            if (W <= 0 || H <= 0) return;

            toolbar.coord = XYWH(0, 0, W, h);
            button_save.coord = XYWH(0, 0, w, h);
            button_run .coord = XYWH(W/2-w/2, 0, w, h);
            button_test.coord = XYWH(W-w, 0, w, h);

            int d = gui::metrics::line::width * 4;
            int l = sys::settings::load("splitter.editor.l.permyriad", 18'00) * W / 100'00;
            int r = sys::settings::load("splitter.editor.r.permyriad", 70'00) * W / 100'00;

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

        if (what == &timer)
        {
            auto ext = editor.object.path.now.extension();
            button_run.enabled = ext == ".ae!" &&
                library_status == status::ready;
        }
    }


    void on_notify (gui::base::widget* w) override
    {
        if (w == &button_test)
        {
            test.show (test.alpha.to == 0, gui::time(500));
        //  flist.show (test.alpha.to != 0, gui::time(500));
        //  editor.show (test.alpha.to != 0, gui::time(500));
        //  console.show (test.alpha.to != 0, gui::time(500));
        }

        if (w == &flist)
        {
            editor.object.load(flist.object.selected.now);
        }
        if (w == &editor.object.flist)
        {
            editor.object.load(editor.object.flist.selected.now);
        }
        if (w == &editor)
        {
            console.object.console.clear();
            if (str log = editor.object.editor.model.log(); log != "")
                console.object.console << log;
        }
        if (w == &button_run)
        {
            console.object.console.clear();
            console.object.console << "<b><font color=#000080> Run... </font></b>";
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
sys::app<IDE> app("AE proto-studio");



