#pragma once
#include "gui.h"
#include "doc_ae_model.h"
#include "doc_cpp_model.h"
#include "doc_text_model.h"
#include "doc_text_repo.h"
#include "app_ide_edflist.h"
using path = std::filesystem::path;
using namespace pix;

struct Editor : gui::widget<Editor>
{
    edflist flist;
    gui::text::view lineup;
    gui::text::editor editor;
    gui::binary_property<path> path;
    doc::text::model* model = nullptr;
    doc::text::report log;

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            int h = gui::metrics::text::height;

            lineup.canvas.color = gui::skins[skin].ultralight.first;
            lineup.alignment = XY{pix::right, pix::top};
            lineup.rpadding.now = h / 4;
            lineup.wordwrap = false;
            lineup.style = pix::text::style{
                sys::font{"Consolas", h},
                RGBA::teal};

            editor.virtual_space = true;
            editor.page.view.wordwrap = false;
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

            using pix::text::style_index;
            auto s = editor.page.style.now;
            auto& ss = doc::model::styles;
            s.color = RGBA::black;   ss["name"     ] = style_index(s);
            s.color = RGBA::blue;    ss["keyword"  ] = style_index(s);
            s.color = RGBA::teal;    ss["keyname"  ] = style_index(s);
            s.color = RGBA::blue;    ss["pragma"   ] = style_index(s);
            s.color = RGBA::purple;  ss["macros"   ] = style_index(s);
            s.color = RGBA::purple;  ss["module"   ] = style_index(s);
            s.color = RGBA::navy;    ss["number"   ] = style_index(s);
            s.color = RGBA::white;   ss["space"    ] = style_index(s); 
            s.color = RGBA::navy;    ss["literal"  ] = style_index(s); 
            s.color = RGBA::navy;    ss["char"     ] = style_index(s); 
            s.color = RGBA::maroon;  ss["symbol"   ] = style_index(s);
            s.color = RGBA::maroon;  ss["semicolon"] = style_index(s);
            s.color = RGBA::fuchsia; ss["comment"  ] = style_index(s);
            s.color = RGBA::red;     ss["error"    ] = style_index(s);
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

            editor.model = model = 
                ext == "ae"  ? doc::text::repo::load<doc::ae::model>(path.now):
                ext == "cpp" ? doc::text::repo::load<doc::cpp::model>(path.now):
                               doc::text::repo::load<doc::text::model>(path.now);
            editor.update_text = true;
        }

        if (what == &editor.text)
        {
            doc::text::repo::edit(path.now);

            int n1 = lineup.rows();
            int n2 = editor.rows();
            if (n1 != n2) {
                str text;
                text.reserve(n2 * (int)(std::log10(n2))); for (int i = 0; i < n2; i++)
                    text += std::to_string(i + 1) + (char*)(u8"\u00A0") + "\n"; // &nbsp;
                text.truncate();
                lineup.text = text;
            }

            notify();
        }

        if (what == &flist) {
            path = flist.selected.now;
            notify(&flist);
        }

        if (what == &editor.page.scroll.y) lineup.shift =
            XY(0, -editor.page.scroll.y.top);
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_key_input (str symbol) override { editor.on_key_input(symbol); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }

    bool syntax_ready ()
    {
        if (model and
            model->ready()) { log =
            model->log();
            //auto t0 = doc::ae::syntax::analysis::now();
            editor.update_colors = true;
            //auto t1 = doc::ae::syntax::analysis::now();
            //auto ms = doc::ae::syntax::analysis::ms(t1-t0);
            //doc::ae::syntax::analysis::events.debug("syntax_ready " + ms + " ms");
            return true;
        }

        return false;
    }
};
