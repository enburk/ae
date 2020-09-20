#pragma once
#include "doc.h"
#include "doc_text_repo.h"
#include "doc_cpp_syntax.h"
#include "doc_ae_syntax_analysis.h"
#include "gui_widget_button.h"
#include "gui_widget_scroller.h"
#include "gui_widget_text_editor.h"
using namespace pix;

struct EditorFlist : gui::widget<EditorFlist>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    gui::binary_property<std::filesystem::path> selected;
    struct data { std::filesystem::path path; long ago = 0; };
    array<data> datae;

    void load (std::filesystem::path path)
    {
        if (auto it = datae.find_if([path](auto d)
            { return d.path == path; });
            it != datae.end())
        {
            selected = it->path;
            for (auto & d : datae) if (d.ago < it->ago) d.ago++; it->ago = 0;
            for (auto & button : buttons) button.on = false;
            buttons((int)(it - datae.begin())).on = true;
            on_change(&coord); // could be out of view now
        }
        else
        {
            selected = path;
            for (auto & d : datae) d.ago++;
            for (auto & button : buttons) button.on = false;
            datae += data { path, 0 };
            buttons.emplace_back();
            buttons.back().text.text = path.filename().string();
            buttons.back().on = true;
            on_change(&coord);
        }
    }

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            canvas.coord = coord.now.local();
            buttons.coord = coord.now.local();
            if (datae.size() == 0) return;

            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height;
            const int cell_width_in_heights = 15;
            int w = cell_width_in_heights * h;
            int n = W/w; if (n == 0) n = 1;

            // std::stable_partition... sort of...
            for (int i=0; i<min(n, datae.size());)
            {
                if (datae[i].ago < n) i++; else {
                    std::rotate(datae.begin()+i, datae.begin()+i+1, datae.end());
                    buttons.rotate(i, i+1, buttons.size());
                }
            }
            for (int i=0; i<buttons.size(); i++)
            {
                buttons(i).show(datae[i].ago < n);
                buttons(i).coord = XYWH(i*w, 0, w, H);
            }
        }
        if (what == &skin)
        {
            canvas.color = gui::skins[skin.now].light.back_color;
        }
    }

    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &buttons && selected.now != datae[n].path) {
            selected = datae[n].path;
            notify();
        }
    }
};

struct Editor : gui::widget<Editor>
{
    str filename;

    EditorFlist flist;
    gui::text::view lineup;
    gui::text::editor editor;
    gui::binary_property<std::filesystem::path> path;
    doc::report log;

    void save ()
    {
        doc::repo::modify(path.now, editor.model);
        auto rc = doc::repo::save(path.now);
        if (!rc.ok()) log.error(nullptr,
            rc.error());
    }
    void save_all ()
    {
        auto rc = doc::repo::save_all();
        if (!rc.ok()) log.error(nullptr,
            rc.error());
    }
    void reload ()
    {
        auto rc = doc::repo::reload();
        if (!rc.ok()) log.error(nullptr, rc.error());
        internal_load();
    }

    void internal_load ()
    {
        auto rc = doc::repo::load(path.now, editor.model);
        if (!rc.ok()) log.error(nullptr, rc.error());

        str ext = path.now.extension().string();
        if (ext == ".cpp" || ext == ".hpp"
        ||  ext == ".cxx" || ext == ".hxx"
        ||  ext == ".c++" || ext == ".h++"  || ext == ".h") ext = "cpp"; else
        if (ext == ".ae!" || ext == ".ae!!" || ext == ".ae") ext = "ae";

        editor.model.proceed = [this, ext] (const doc::text& text, array<doc::token>& tokens)
        {
            tokens = 
                ext == "ae"  ? doc::ae::lexica ::parse(text):
                ext == "cpp" ? doc::lexica::cpp::parse(text):
                               doc::lexica::txt::parse(text);
            log = 
                ext == "ae"  ? doc::ae ::syntax::analysis::proceed(path.now, tokens).log:
                ext == "cpp" ? doc::cpp::syntax::analysis::proceed(path.now, tokens).log:
                               doc::report{};
        };
        editor.reset();
    }

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height * 12/7;
            int d = 3*h;

            flist .coord = XYXY(0, 0, W, h);
            lineup.coord = XYXY(0, h, d, H);
            editor.coord = XYXY(d, h, W, H);
        }
        if (what == &skin)
        {
            int h = gui::metrics::text::height;

            lineup.ground.color = gui::skins[skin.now].ultralight.back_color;
            lineup.alignment = XY{gui::text::right, gui::text::top};
            lineup.word_wrap = false;
            lineup.style = sys::glyph_style{
                sys::font{"Consolas", h},
                pix::teal};

            editor.virtual_space = true;
            editor.page.view.word_wrap = false;
            editor.page.scroll.x.mode = gui::scroll::mode::none;
            editor.page.view.ground.color = pix::white;
            editor.page.style = sys::glyph_style{
                sys::font{"Consolas", h},
                pix::black};

            auto s = editor.page.style.now;
            s.color = pix::black;   editor.styles["name"   ] = sys::glyph_style_index(s);
            s.color = pix::blue;    editor.styles["keyword"] = sys::glyph_style_index(s);
            s.color = pix::teal;    editor.styles["keyname"] = sys::glyph_style_index(s);
            s.color = pix::blue;    editor.styles["pragma" ] = sys::glyph_style_index(s);
            s.color = pix::purple;  editor.styles["macros" ] = sys::glyph_style_index(s);
            s.color = pix::navy;    editor.styles["number" ] = sys::glyph_style_index(s);
            s.color = pix::white;   editor.styles["space"  ] = sys::glyph_style_index(s); 
            s.color = pix::navy;    editor.styles["literal"] = sys::glyph_style_index(s); 
            s.color = pix::navy;    editor.styles["char"   ] = sys::glyph_style_index(s); 
            s.color = pix::maroon;  editor.styles["symbol" ] = sys::glyph_style_index(s);
            s.color = pix::fuchsia; editor.styles["comment"] = sys::glyph_style_index(s);
            s.color = pix::red;     editor.styles["error"  ] = sys::glyph_style_index(s);
        }
        if (what == &path)
        {
            flist.load(path.now);

            doc::repo::modify(path.was, editor.model);

            internal_load();
        }
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }

    void on_notify (gui::base::widget* w) override
    {
        if (w == &editor)
        {
            doc::repo::modify(path.now, editor.model);

            int n1 = lineup.model->lines.size();
            int n2 = editor.model.lines.size();
            if (n1 != n2) {
                str text;
                text.reserve(n2*(int)(std::log10(n2)));
                for (int i=0; i<n2; i++)
                    text += std::to_string(i+1) + (char*)(u8"\u00A0") + "\n"; // &nbsp;
                text.truncate();
                lineup.text = text;
            }

            notify();
        }

        if (w == &flist)
        {
            notify(&flist);
        }
    }
    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &editor.page.scroll.y)
            lineup.shift = XY(0, n);
    }
};

// struct text-model: selections, undoes/redoes, but no tokens
// struct key-processor: modify text-model
// struct 