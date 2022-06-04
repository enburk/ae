#pragma once
#include "../../auxs/test.h"
#include "app_ide_compiler.h"
#include "app_ide_console.h"
#include "app_ide_editor.h"
#include "app_ide_flist.h"
using namespace std::literals::chrono_literals;

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

    std::thread thread;
    gui::property<gui::time> timer;
    sys::directory_watcher watcher;
    std::atomic<bool> reload = false;
    gui::time edittime;
    bool syntax_run = false;
    bool syntax_ok = false;

    IDE()
    {
        canvas.color = rgba::red;
        toolbar.color = gui::skins[skin].light.first;
        button_run .text.text = "run";
        button_test.text.text = "test";
        console.activate(&console.events);
        test_area.hide();

        watcher.dir = std::filesystem::current_path();
        watcher.action = [this](std::filesystem::path path, str what)
        {
            str s = path.string();
            if (s.contains("\\.vs\\")) return;
            if (s.contains("\\.astudio\\")) return;
            if (s.contains("\\.vstudio\\")) return;
            if (s.contains("\\enc_temp_folder\\")) return;
            if (s.ends_with(".ae!.cpp") || s.ends_with(".ae!!.cpp")) return;
            if (s.ends_with(".ae!.obj") || s.ends_with(".ae!!.obj")) return;
            if (s.ends_with(".ae!.exe") || s.ends_with(".ae!!.exe")) return;
            if (s.ends_with("cl.log.txt")) return;
            console.events << "<font color=#9E9E9E>"
            + path.string() + " " + what + "</font>";
            reload = true;
        };
        watcher.error = [this](aux::error error){
        console.events << "<font color=#B00020>"
       "watcher error: " + error + "</font>"; };
        watcher.watch();
    }
    ~IDE()
    {
        watcher.cancel();
        doc::text::repo::save();
        if (thread.joinable())
            thread.join();
    }

    void on_change (void* what) override
    {
        if (timer.now == gui::time())
            timer.go(gui::time::infinity,
                     gui::time::infinity);

        if (what == &timer)
        {
            if (reload) {
                reload = false;
                flist.reload();
                editor.flist.reload(); // before repo delete something
                doc::text::repo::reload(); // triggers recompiling
                editor.editor.update_text = true;
                syntax_run = true;
                syntax_ok = false;
            }

            if ((gui::time::now - edittime) > 30s) {
                edittime = gui::time::infinity;
                doc::text::repo::save();
            }

            if ((gui::time::now - edittime) > 0s) {
                auto & report = doc::text::repo::report;
                if (report.errors.size() > 0) edittime = gui::time::now;
                if (report.messages.size() > 0) {
                    console.activate(&console.events);
                    console.events << report();
                    report.clear();
                }
            }
            
            if ((gui::time::now - edittime) > 10s) {
                auto & report = doc::ae::syntax::analysis::events;
                if (report.messages.size() > 0) {
                    console.events << report();
                    report.clear();
                }
            }

            doc::text::repo::tick();

            if ((gui::time::now - edittime) > 500ms)
            if (syntax_run and editor.syntax_ready()) {
                syntax_run = false;
                syntax_ok = editor.log.errors.size() == 0;
                console.editor.clear();
                if (editor.log.messages.size() > 0) {
                    console.activate(&console.editor);
                    console.editor << editor.log();
                    editor.log.clear();
                }
            }

            button_run.enabled = (
                editor.path.now.extension() == ".ae!" or
                editor.path.now.extension() == ".ae!!")
                and syntax_ok;
        }

        if (what == &coord)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*12/7;
            
            int d = gui::metrics::line::width * 6;
            int l = sys::settings::load("splitter.editor.l.permyriad", 18'00) * W / 100'00;
            int r = sys::settings::load("splitter.editor.r.permyriad", 70'00) * W / 100'00;

            splitter_editor_l.coord = xyxy(l-d, h, l+d, H);
            splitter_editor_r.coord = xyxy(r-d, h, r+d, H);
            splitter_editor_l.lower = 10'00 * W / 100'00;
            splitter_editor_l.upper = 35'00 * W / 100'00;
            splitter_editor_r.lower = 55'00 * W / 100'00;
            splitter_editor_r.upper = 85'00 * W / 100'00;

            canvas.coord = coord.now.local();
            toolbar.coord = xywh(0, 0, W, h);
            button_run .coord = xywh(0, 0, w, h);
            button_test.coord = xywh(W-w, 0, w, h);

            test_area.coord = xyxy(0, h, W, H);
            flist_area.coord = xywh(0, h, l-0, H-h);
            editor_area.coord = xywh(l, h, r-l, H-h);
            console_area.coord = xywh(r, h, W-r, H-h);
        }

        if (test_area.object.aux.done) button_test.text.color = 
            test_area.object.aux.ok ? rgba::green : rgba::error;

        if (what == &flist)
        {
            editor.flist.selected = flist.selected.now;
            syntax_run = true;
            syntax_ok = false;
            focus = &editor;
        }

        if (what == &editor.flist)
        {
            flist.selected = editor.flist.selected.now;
            syntax_run = true;
            syntax_ok = false;
            focus = &editor;
        }
        if (what == &editor)
        {
            edittime = gui::time::now;
            syntax_run = true;
            syntax_ok = false;
        }

        if (what == &console)
        {
            if (syntax_run) return;

            std::string source = console.pressed_file;
            if (source != "" and std::filesystem::exists(source))
                editor.path = source;

            editor.editor.go(doc::place{
                std::stoi(console.pressed_line)-1,
                std::stoi(console.pressed_char)-1});

            focus = &editor;
        }

        if (what == &button_test)
        {
            bool on = test_area.alpha.to != 0;
            bool turn_on = not on;
            test_area.show(turn_on);
            test_area.coord = turn_on ?
                xyxy(0, toolbar.coord.now.h, coord.now.w, coord.now.h):
                xyxy{};

            if (turn_on)
                focus = &test_area; else
                focus = &editor_area;
        }

        if (what == &button_run)
        {
            console.activate(&console.output);

            if (not ide::compiler::translate(
                editor.path.now,
                console.output))
                return;

            if (thread.joinable())
                thread.join();

            thread = std::thread([this]()
            {
                ide::compiler::run(
                    editor.path.now,
                    console.output);
            });
        }

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
    }
};
sys::app<IDE> app("ae proto-studio");//, {0,0}, {100, 100});



