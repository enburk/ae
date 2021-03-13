#pragma once
#include "doc_ae_model.h"
#include "doc_cpp_model.h"
#include "doc_text_model.h"
#include "doc_text_repo.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "gui_widget_text_editor.h"
#include "app_ide_edflist.h"
using path = std::filesystem::path;
using namespace pix;

struct Editor : gui::widget<Editor>
{
    edflist flist;
    gui::text::view lineup;
    gui::text::editor editor;
    gui::binary_property<path> path;
    doc::text::report log;

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            int h = gui::metrics::text::height;

            lineup.canvas.color = gui::skins[skin].ultralight.first;
            lineup.alignment = XY{pix::right, pix::top};
            lineup.word_wrap = false;
            lineup.style = pix::text::style{
                sys::font{"Consolas", h},
                RGBA::teal};

            editor.virtual_space = true;
            editor.page.view.word_wrap = false;
            editor.page.scroll.x.mode = gui::scroll::mode::none;
            editor.page.view.current_line_frame.color = pix::ARGB(0x40909090);
            editor.page.view.canvas.color = RGBA::white;
            editor.page.style = pix::text::style{
                sys::font{"Consolas", h},
                RGBA::black};

            editor.page.info.canvas.color = gui::skins[skin].light.first;
            editor.page.info.frame.color = gui::skins[skin].heavy.first;
            editor.page.info.style = pix::text::style{
                sys::font{"Consolas", h},
                gui::skins[skin].dark.first};

            auto s = editor.page.style.now;
            s.color = RGBA::black;   editor.styles["name"     ] = pix::text::style_index(s);
            s.color = RGBA::blue;    editor.styles["keyword"  ] = pix::text::style_index(s);
            s.color = RGBA::teal;    editor.styles["keyname"  ] = pix::text::style_index(s);
            s.color = RGBA::blue;    editor.styles["pragma"   ] = pix::text::style_index(s);
            s.color = RGBA::purple;  editor.styles["macros"   ] = pix::text::style_index(s);
            s.color = RGBA::purple;  editor.styles["module"   ] = pix::text::style_index(s);
            s.color = RGBA::navy;    editor.styles["number"   ] = pix::text::style_index(s);
            s.color = RGBA::white;   editor.styles["space"    ] = pix::text::style_index(s); 
            s.color = RGBA::navy;    editor.styles["literal"  ] = pix::text::style_index(s); 
            s.color = RGBA::navy;    editor.styles["char"     ] = pix::text::style_index(s); 
            s.color = RGBA::maroon;  editor.styles["symbol"   ] = pix::text::style_index(s);
            s.color = RGBA::maroon;  editor.styles["semicolon"] = pix::text::style_index(s);
            s.color = RGBA::fuchsia; editor.styles["comment"  ] = pix::text::style_index(s);
            s.color = RGBA::red;     editor.styles["error"    ] = pix::text::style_index(s);
        }
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*12/7;
            int d = 3*h;

            flist .coord = XYXY(0, 0, W, h);
            lineup.coord = XYXY(0, h, d, H);
            editor.coord = XYXY(d, h, W, H);
        }
        if (what == &path)
        {
            str ext = path.now.extension().string();
            if (ext == ".cpp" || ext == ".hpp"
            ||  ext == ".cxx" || ext == ".hxx"
            ||  ext == ".c++" || ext == ".h++"  || ext == ".h" ) ext = "cpp"; else
            if (ext == ".ae!" || ext == ".ae!!" || ext == ".ae") ext = "ae";

            editor.model = 
                ext == "ae"  ? doc::text::repo::load<doc::ae::model>(path.now):
                ext == "cpp" ? doc::text::repo::load<doc::cpp::model>(path.now):
                               doc::text::repo::load<doc::text::model>(path.now);
            editor.reset();
        }
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }

    void on_notify (void* what) override
    {
        if (what == &editor)
        {
            doc::text::repo::edit(path.now);

            int n1 = lineup.model->lines.size();
            int n2 = editor.model->lines.size();
            if (n1 != n2) {
                str text;
                text.reserve(n2*(int)(std::log10(n2))); for (int i=0; i<n2; i++)
                text += std::to_string(i+1) + (char*)(u8"\u00A0") + "\n"; // &nbsp;
                text.truncate();
                lineup.text = text;
            }

            notify();
        }

        if (what == &flist) {
            path = flist.selected.now;
            notify (&flist);
        }

        if (what == &editor.page.scroll.y) lineup.shift =
            XY(0, -editor.page.scroll.y.top);
    }

    bool syntax_ready ()
    {
        if (editor.model->ready()) { log =
            editor.model->log();
            //auto t0 = doc::ae::syntax::analysis::now();
            editor.page.view.update();
            editor.page.view.refresh();
            editor.page.refresh();
            editor.refresh();
            //auto t1 = doc::ae::syntax::analysis::now();
            //auto ms = doc::ae::syntax::analysis::ms(t1-t0);
            //doc::ae::syntax::analysis::events.debug("syntax_ready " + ms + " ms");
            return
            true;
        }

        return false;
    }
};
