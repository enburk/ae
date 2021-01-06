#pragma once
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "gui_widget_text_editor.h"
using path = std::filesystem::path;
using namespace pix;

struct edflist : gui::widget<edflist>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    typedef std::filesystem::path path;
    gui::binary_property<path> selected;
    struct data { path path; long ago = 0; };
    array<data> flist;

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            canvas.color = gui::skins[skin].light.first;
        }
        if (what == &coord)
        {
            canvas .coord = coord.now.local();
            buttons.coord = coord.now.local();
            refresh();
        }
        if (what == &selected)
        {
            path path = selected.now;
            if (auto it = flist.find_if([path](auto d)
                { return d.path == path; });
                it != flist.end())
            {
                selected = it->path;
                for (auto & d : flist) if (d.ago < it->ago) d.ago++; it->ago = 0;
                for (auto & button : buttons) button.on = false;
                buttons((int)(it - flist.begin())).on = true;
                on_change(&coord); // could be out of view now
            }
            else
            {
                selected = path;
                for (auto & d : flist) d.ago++;
                for (auto & button : buttons) button.on = false;
                flist += data { path, 0 };
                buttons.emplace_back();
                buttons.back().text.text = path.filename().string();
                buttons.back().on = true;
                on_change(&coord);
            }
            notify();
        }
    }

    void refresh ()
    {
        int W = coord.now.w; if (W <= 0) return;
        int H = coord.now.h; if (H <= 0) return;
        int w = 15 * gui::metrics::text::height;
        int n = W/w; if (n == 0) n = 1; w = W/n;
        
        for (int i=0; i<min(n, flist.size());)
        {   // stable_partition... sort of...
            if (flist[i].ago < n) i++; else {
                std::rotate(flist.begin()+i, flist.begin()+i+1, flist.end());
                buttons.rotate(i, i+1, buttons.size());
            }
        }
        for (int i=0; i<buttons.size(); i++)
        {
            buttons(i).show(flist[i].ago < n);
            buttons(i).coord = XYWH(i*w, 0, w, H);
        }
    }

    void on_notify (void* what) override
    {
        if (what == &buttons) selected =
            flist[buttons.notifier_index].path;
    }
};

struct Editor : gui::widget<Editor>
{
    edflist flist;
    gui::text::view lineup;
    gui::text::editor editor;
    gui::binary_property<path> path;

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
            editor.page.view.canvas.color = RGBA::white;
            editor.page.style = pix::text::style{
                sys::font{"Consolas", h},
                RGBA::black};
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
            flist.selected = path.now;
        }
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }

    void on_notify (void* what) override
    {
        if (what == &editor)
        {
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

        if (what == &flist)
            notify (&flist);

        if (what == &editor.page.scroll.y) lineup.shift =
            XY(0, editor.page.scroll.y.top);
    }
};
