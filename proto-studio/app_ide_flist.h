#pragma once
#include "abc.h"
#include <filesystem>
using path = std::filesystem::path;

struct Flist:
widget<Flist>
{
    struct contents: sfx::dirtree
    {
        contents ()
        {
            sfx::dirtree::filter = [](path path)
            {
                static array<str> exts = 
                {
                    ".h",
                    ".hpp", ".cpp",
                    ".h++", ".c++",
                    ".inc", ".txt",
                    ".ae", ".ae!", ".ae!!"
                };
                return
                    std::filesystem::is_directory(path) and not
                    path.filename().string().starts_with(".")
                    or exts.contains(
                    path.extension().
                    string());
            };
            sfx::dirtree::contents.pretty = [](record r)
            {
                return
                    r.name.ends_with(".h"  ) or
                    r.name.ends_with(".hpp") or
                    r.name.ends_with(".cpp") or
                    r.name.ends_with(".inc") or
                    r.name.ends_with(".txt") ? gray(r.name) :
                    r.name.ends_with(".h++") or
                    r.name.ends_with(".c++") ? dark(r.name) :
                    r.file ? r.name :
                        bold(r.name);
            };
        }
    };

    gui::toolbar canvas;
    gui::text::view dir;
    contents contents;

    binary_property<path> root;
    binary_property<path> selected;

    void on_change (void* what) override
    {
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*13/10;

            canvas  .coord = xyxy(0, 0, W, H);
            dir     .coord = xyxy(0, 0, W, h);
            contents.coord = xyxy(0, h, W, H);
        }

        if (what == &name)
            contents.name =
            name.now +
            "::contents";

        if (what == &root)
            dir.text = root.now.string(),
            contents.root =
            root;

        if (what == &contents)
            selected = contents.
            selected,
            notify();
    }
};
