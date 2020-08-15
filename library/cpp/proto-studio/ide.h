#pragma once
#include <experimental/coroutine>
#include "gui_window.h"
#include "gui_widget_image.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "gui_widget_splitter.h"
#include "doc_text_repo.h"
#include "ide_compiler.h"
#include "ide_console.h"
#include "ide_editor.h"
#include "ide_flist.h"
#include "test.h"
using namespace pix;

struct IDE : gui::widget<IDE>
{
    gui::canvas toolbar;
    gui::button button_save;
    gui::button button_Save;
    gui::button button_run;
    gui::button button_test;

    gui::area<Console> console;
    gui::area<Editor> editor;
    gui::area<Flist> flist;
    gui::area<Test> test;

    gui::splitter splitter_editor_l;
    gui::splitter splitter_editor_r;

    bool syntax_ok = false;
    gui::property<gui::time> timer;
    gui::time last_compile_time;
    std::thread thread;
    std::atomic<bool> finished = true;

    sys::directory_watcher watcher;
    std::atomic<bool> reload_editor = false;
    std::atomic<bool> reload_flist = false;

    IDE()
    {
        gui::window = this; skin = "gray";

        button_save.text.text = "Save";
        button_Save.text.text = "Save All";
        button_run .text.text = "Run";
        button_test.text.text = "Test";
        button_save.enabled = false;
        button_Save.enabled = false;
        button_run .enabled = false;
        button_test.kind = gui::button::toggle;
        test.hide();

        console.object.activate(&console.object.compiler);

        finished = false;

        thread = std::thread([this]()
        {
            for (std::filesystem::directory_iterator next
                (std::filesystem::current_path() / "library"),
                end; next != end; ++next)
            {
                auto path = next->path();
                if (std::filesystem::is_regular_file (path) &&
                    path.extension() == ".ae")
                {
                    ide::compiler::add_std_library(path);
                }
            }

            ide::compiler::prepare(console.object.compiler);

            finished = true;
        });

        watcher.dir = std::filesystem::current_path();
        watcher.action = [this](std::filesystem::path path, str what)
        {
            str s = path.string();
            if (s.contains("\\.vs\\")) return;
            if (s.contains("\\.vstudio\\")) return;
            if (s.contains("\\enc_temp_folder\\")) return;
            if (s.ends_with(".ae!.cpp") or s.ends_with(".ae!!.cpp")) return;
            if (s.ends_with(".ae!.obj") or s.ends_with(".ae!!.obj")) return;
            if (s.ends_with(".ae!.exe") or s.ends_with(".ae!!.exe")) return;
            if (s.ends_with("cl.log.txt")) return;
            console.object.compiler << path.string() + " " + what;
            reload_editor = true; if (what != "modified")
            reload_flist = true;
        };
        watcher.error = [this](aux::error error){
            console.object.compiler << "<font color=#B00020>"
                "watcher error: " + error + "</font>";
        };
        watcher.watch();
    }
    ~IDE()
    {
        if (thread.joinable())
            thread.join();
    }

    void on_close () override { doc::repo::close(); }

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
            button_Save.coord = XYWH(w, 0, w, h);
            button_run .coord = XYWH(W/2-w/2, 0, w, h);
            button_test.coord = XYWH(W-w, 0, w, h);

            int d = gui::metrics::line::width * 6;
            int l = sys::settings::load("splitter.editor.l.permyriad", 18'00) * W / 100'00;
            int r = sys::settings::load("splitter.editor.r.permyriad", 70'00) * W / 100'00;

            splitter_editor_l.coord = XYXY(l-d, h, l+d, H);
            splitter_editor_r.coord = XYXY(r-d, h, r+d, H);
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
            if (reload_editor) {
                reload_editor = false;
                editor.object.reload();
            }
            if (reload_flist) {
                reload_flist = false;
                flist.object.reload();
            }

            auto ext = editor.object.path.now.extension();
            button_run .enabled = ext == ".ae!" and syntax_ok;

            auto any_one = doc::repo::saveable::any_one(editor.object.path.now);
            button_save.enabled = any_one.second;
            button_Save.enabled = any_one.first;

            // using namespace std::literals::chrono_literals;
            // if (timer.now - last_compile_time > 5s
            //     and button_run.enabled
            //     and finished)
            // {
            //     if (thread.joinable())
            //         thread.join();
            // 
            //     last_compile_time = timer.now;
            // 
            //     finished = false;
            //     thread = std::thread([this]()
            //     {
            //         ide::compiler::compile(
            //             editor.object.path.now,
            //             console.object.compiler);
            //         finished = true;
            //     });
            // }
        }
    }

    void on_notify (gui::base::widget* w) override
    {
        if (w == &button_test)
        {
            test.show (test.alpha.to == 0, gui::time(500));
        }

        if (w == &flist)
        {
            editor.object.path = flist.object.selected.now;
        }
        if (w == &editor.object.flist)
        {
            editor.object.path = editor.object.flist.selected.now;
        }
        if (w == &editor)
        {
            syntax_ok = true;
            console.object.editor.clear();
            if (str log = editor.object.log(); log != "") {
                console.object.activate(&console.object.editor);
                console.object.editor << log;
                syntax_ok = false;
            }
            last_compile_time = gui::time::now;
        }
        if (w == &console)
        {
            std::string source = console.object.pressed_file;
            if (source != "" and std::filesystem::exists(source))
                editor.object.path = source;

            editor.object.editor.go(doc::place{
                std::stoi(console.object.pressed_line)-1,
                std::stoi(console.object.pressed_char)-1});
        }
        if (w == &button_save)
        {
            auto rc = doc::repo::save(editor.object.path.now); if (!rc.ok()) {
                console.object.activate(&console.object.compiler);
                console.object.compiler << "<font color=#B00020>"
                "Save: " + rc.error() + "</font>";
            }
        }
        if (w == &button_Save)
        {
            auto rc = doc::repo::save_all(); if (!rc.ok()) {
                console.object.activate(&console.object.compiler);
                console.object.compiler << "<font color=#B00020>"
                "Save All: " + rc.error() + "</font>";
            }
        }
        if (w == &button_run)
        {
            console.object.activate(&console.object.compiler);

            if (thread.joinable())
                thread.join();

            last_compile_time = gui::time::now;

            finished = false;
            thread = std::thread([this]()
            {
                ide::compiler::run(editor.object.path.now,
                    console.object.compiler);
                finished = true;
            });
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



