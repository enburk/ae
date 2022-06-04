#pragma once
#include "abc.h"
#include "doc_ae_model.h"
#include "doc_text_repo.h"
#include "app_ide_edflist.h"
using path = std::filesystem::path;
using namespace pix;

struct Editor : gui::widget<Editor>
{
    edflist flist;
    gui::text::page lineup;
    gui::text::editor editor;
    gui::binary_property<path> path;
    doc::text::model* model = nullptr;
    doc::text::report log;

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            lineup.scroll.x.mode = gui::scroll::mode::none;
            lineup.scroll.y.mode = gui::scroll::mode::none;
            lineup.canvas.color = gui::skins[skin].ultralight.first;
            lineup.padding = xyxy{0,0,gui::metrics::line::width*5,0};
            lineup.alignment = xy{pix::right, pix::top};
            lineup.font = pix::font{"Consolas"};
            lineup.color = rgba::teal;
            lineup.wordwrap = false;

            editor.virtual_space = true;
            editor.view.wordwrap = false;
            editor.scroll.x.mode = gui::scroll::mode::none;
            editor.view.current_line_frame.color = rgba(150,150,150,64);
            editor.padding = xyxy{gui::metrics::line::width*5,0,0,0};
            editor.font = pix::font{"Consolas"};
            editor.canvas.color = rgba::white;
            editor.color = rgba::black;

            using pix::text::style_index;
            auto s = editor.view.style.now;
            auto& ss = doc::model::styles;
            s.color = rgba::black;   ss["name"     ] = style_index(s);
            s.color = rgba::blue;    ss["keyword"  ] = style_index(s);
            s.color = rgba::teal;    ss["keyname"  ] = style_index(s);
            s.color = rgba::blue;    ss["pragma"   ] = style_index(s);
            s.color = rgba::purple;  ss["macros"   ] = style_index(s);
            s.color = rgba::purple;  ss["module"   ] = style_index(s);
            s.color = rgba::navy;    ss["number"   ] = style_index(s);
            s.color = rgba::white;   ss["space"    ] = style_index(s); 
            s.color = rgba::navy;    ss["literal"  ] = style_index(s); 
            s.color = rgba::navy;    ss["char"     ] = style_index(s); 
            s.color = rgba::maroon;  ss["symbol"   ] = style_index(s);
            s.color = rgba::maroon;  ss["semicolon"] = style_index(s);
            s.color = rgba::fuchsia; ss["comment"  ] = style_index(s);
            s.color = rgba::red;     ss["error"    ] = style_index(s);
        }
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*12/10;
            int d = 3*h;

            flist .coord = xyxy(0, 0, W, h);
            lineup.coord = xyxy(0, h, d, H);
            editor.coord = xyxy(d, h, W, H);
        }
        if (what == &path)
        {
            str ext = path.now.extension().string();
            if (ext == ".cpp" || ext == ".hpp"
            ||  ext == ".cxx" || ext == ".hxx"
            ||  ext == ".c++" || ext == ".h++"  || ext == ".h" ) ext = "cpp"; else
            if (ext == ".ae!" || ext == ".ae!!" || ext == ".ae") ext = "ae";

            editor.model = model = 
            ext == "ae" ? doc::text::repo::load<doc::ae::model>(path.now):
            ext == "cpp"? doc::text::repo::load<doc::cpp::model>(path.now):
                          doc::text::repo::load<doc::text::model>(path.now);
            editor.update_text = true;
        }

        if (what == &editor.update_text)
        {
            doc::text::repo::edit(path.now);

            int n1 = lineup.rows();
            int n2 = editor.rows();
            if (n1 != n2) {
                str text;
                text.reserve(n2*10); for (int i=0; i<n2; i++)
                text += std::to_string(i+1) + "\n";
                text.truncate();
                lineup.text = text;
            }

            notify();
        }

        if (what == &flist) {
            path = flist.selected.now;
            notify(&flist); }

        if (what == &editor.scroll.y)
            lineup.view.shift = xy(0,
                -editor.scroll.y.top);
    }

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
