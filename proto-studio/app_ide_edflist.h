#pragma once
#include <filesystem>
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
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
        int w = 12 * gui::metrics::text::height;
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

    void reload ()
    {
        for (int i=0; i<flist.size(); )
        {
            if (std::filesystem::exists(
            flist[i].path)) { i++; continue; }
            flist.erase(flist.begin()+i);
            buttons.erase(i);
        }

        for (int i=0; i<flist.size(); i++)
            if (selected.now == flist[i].path)
                return;

        selected = flist.size() > 0 ? 
            flist[0].path :
            path{};
    }

    void on_notify (void* what) override
    {
        if (what == &buttons) selected =
            flist[buttons.notifier_index].path;
    }
};
