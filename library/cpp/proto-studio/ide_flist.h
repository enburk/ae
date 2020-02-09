#pragma once
#include <filesystem>
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
using namespace pix;

struct Flist : gui::widget<Flist>
{
    gui::canvas canvas;
    gui::frame  frame1;
    gui::frame  frame2;

    gui::text::view dir;
    gui::widgetarium<gui::button> subs;
    typedef std::filesystem::path path;
    gui::binary_property<path> root;

    static bool less(path a, path b)
    {
        path pa = a.parent_path();
        path pb = b.parent_path();
        bool da = std::filesystem::is_directory(a);
        bool db = std::filesystem::is_directory(b);
        if ( da && !db) return less(a, pb);
        if (!da &&  db) return less(pa, b);
        if (!da && !db && less(pa, pb)) return true;
        if (!da && !db && less(pb, pa)) return false;
        return a < b;
    }

    void on_change (void* what) override
    {
        using namespace std::filesystem;

        if (what == &coord && coord.was.size != coord.now.size)
        {
            if (root.now == path())
                root = current_path();

            frame1.color = gui::schemas[""].light.back_color;
            frame2.color = gui::schemas[""].heavy.back_color;
            canvas.color = gui::schemas[""].light.back_color;

            auto r = coord.now.local();
            frame1.coord = r; r.deflate(frame1.thickness.now);
            frame2.coord = r; r.deflate(frame2.thickness.now);
            canvas.coord = r; r.deflate(frame2.thickness.now);
            subs  .coord = r;

            int h = 2 * gui::metrics::text::height; int y = 0;

            dir.coord = XYWH(r.x, y, r.w, h); y += h; for (auto & sub : subs) {
            sub.coord = XYWH(r.x, y, r.w, h); y += h; }
        }
        if (what == &root)
        {
            subs.clear();
            array<path> paths;
            for (recursive_directory_iterator next(root.now), end; next != end; ++next)
            {
                path p = next->path();
                if (is_directory (p)) {
                    str name = p.filename().string();
                    if (name.starts_with(".")
                    ||  name.starts_with("_")) { next.disable_recursion_pending(); continue; }
                    paths += p;
                }
                if (is_regular_file (p)) {
                    auto ext = p.extension();
                    if (ext != ".ae!" && ext != ".ae"
                    &&  ext != ".cpp" && ext != ".hpp"
                    &&  ext != ".cxx" && ext != ".hxx"
                    &&  ext != ".c++" && ext != ".h++" && ext != ".h") continue;
                    paths += p;
                }
            }

            paths.sort([](path a, path b) { return less(a,b); });

            for (auto p : paths) {
                if (is_directory (p)) {
                    auto & it = subs.emplace_back();
                    it.text.alignment = XY(gui::text::left, gui::text::center);
                    it.text.text = std::filesystem::relative(p, root.now).string();
                    it.enabled = false;
                }
                if (is_regular_file (p)) {
                    auto & it = subs.emplace_back();
                    it.text.alignment = XY(gui::text::left, gui::text::center);
                    it.text.text = std::filesystem::relative(p, root.now).string();
                }
            }
            dir.text = root.now.string();
        }
    }
}; 