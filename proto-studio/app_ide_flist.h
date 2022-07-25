#pragma once
#include "abc.h"
#include <filesystem>
using path = std::filesystem::path;

struct Flist : gui::widget<Flist>
{
    struct flist : gui::widget<flist>
    {
        int selected = 0;
        gui::radio::group list;
        void on_change (void* w) override {
            selected = list.notifier_index;
            notify ();
        }
    };

    gui::canvas canvas;
    gui::text::view dir; flist flist;
    gui::scroller<gui::vertical> scroller;
    gui::binary_property<path> selected;
    gui::binary_property<path> root;

    void refresh ()
    {
        int W = coord.now.w; if (W <= 0) return;
        int H = coord.now.h; if (H <= 0) return;
        int l = gui::metrics::line::width;
        int d = gui::metrics::text::height*7/10 + 2*l;
        int h = gui::metrics::text::height*12/10;
        int hh = h * flist.list.size();
        int w = W - d;

        if (h + hh <= H) w = W;

        dir.coord = xyxy(0, 0, W, h);
        flist.coord = xyxy(0, h, w, H);
        scroller.coord = xyxy(w, h, W, H);

        int y = 0;
        for (auto & line : flist.list) {
        line.coord = xywh(0, y, w, h);
        y += h; }

        y = flist.list.coord.now.y;
        flist.list.coord = xywh(0, y, w, hh);

        scroller.span = hh;
        scroller.step = h;
    }

    void reload ()
    {
        on_change(&root);
        for (auto& button : flist.list)
            button.on = root.now / std::string(
            button.text.text) ==
            selected.now;
    }

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            canvas.color = gui::skins[skin].light.first;
        }

        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            canvas.coord = coord.now.local();

            if (root.now == path())
                root = std::filesystem::current_path();

            refresh();
        }

        if (what == &root)
        {
            dir.text = root.now.string();
            flist.list.clear();
            fill(root);
            refresh();
        }

        if (what == &selected)
        {
            for (auto& button : flist.list)
                button.on = root.now / std::string(
                button.text.text) ==
                selected.now;

            notify();
        }

        if (what == &flist)
        {
            selected = root / std::string(
                flist.list(flist.selected)
                .text.text);
        }
        if (what == &scroller) { xywh r =
            flist.list.coord; r.y = -scroller.top;
            flist.list.coord = r;
        }
    }

    void fill(path dir)
    {
         int num = 0;
         fill(dir, num);
         flist.list.truncate(num);
    }
    void fill(path dir, int & num)
    {
        using namespace std::filesystem;

        std::map<str, path> files, dirs;

        for (directory_iterator next(dir), end; next != end; ++next)
        {
            path p = next->path();
            if (is_directory (p))
            {
                str name = p.filename().string();
                if (name.starts_with(".")
                or  name.starts_with("_")
                or  name == "enc_temp_folder"
                or  name == "packages") continue;
                dirs[p.filename().string()] =
                    relative(p, root);
            }
            if (is_regular_file (p))
            {
               auto ext = p.extension();
                if (ext != ".ae!" and ext != ".ae!!" and ext != ".ae"
                and ext != ".cpp" and ext != ".hpp"
                and ext != ".cxx" and ext != ".hxx"
                and ext != ".c++" and ext != ".h++"
                and ext != ".h") continue;
                files[p.filename().string()] =
                    relative(p, root);
            }
        }

        for (auto [filename, path] : files) {
            auto & it = flist.list(num++);
            it.text.alignment = xy(pix::left, pix::center);
            it.text.text = path.string();
            it.enabled = true;
        }

        for (auto [filename, path] : dirs) {
            auto & it = flist.list(num++);
            it.text.alignment = xy(pix::left, pix::center);
            it.text.text = path.string();
            it.enabled = false;
            fill(path, num);
        }
    }

    bool on_mouse_wheel (xy p, int delta) override
    {
        int h = scroller.step; if (h <= 0) h = gui::metrics::text::height;
        delta = delta/20 * h; if (delta == 0) delta = delta < 0 ? -h : h;
        if (sys::keyboard::shift) delta *= coord.now.h;
        if (sys::keyboard::ctrl) delta *= 5;
        int d = flist.coord.now.h - flist.list.coord.now.h; // may be negative
        int y = flist.list.coord.now.y + delta;
        if (y < d) y = d;
        if (y > 0) y = 0;
        scroller.top =-y;
        return true;
    }
}; 