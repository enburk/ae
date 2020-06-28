#pragma once
#include "gui_widget_text_page.h"
namespace gui::text
{
    struct console:
    widget<console>
    {
        page page;
        std::mutex mutex;
        property<time> timer;
        array<str> log, addon;

        void operator << (str s)
        {
            std::lock_guard guard{mutex};
            addon += s;
            addon += "<br>";
        }

        void clear ()
        {
            std::lock_guard guard{mutex};
            addon.clear();
            addon += "<script type=\"text/javascript\"></script>"; // HTML NOOP
            log.clear();
        }

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go (time::infinity,
                          time::infinity);

            if (what == &coord && coord.was.size != coord.now.size)
            {
                page.coord = coord.now.local();
            }
            if (what == &skin)
            {
                page.view.ground.color = gui::skins[skin.now].white;
                page.alignment = XY{gui::text::left, gui::text::top};
            }
            if (what == &timer)
            {
                {
                    std::lock_guard guard{mutex};
                    if (addon.size() == 0) return;
                    log += addon;
                    addon.clear();
                }
                page.html = str(log);
                page.scroll.y.top = max<int>();
            }
        }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (parent)
                parent->on_mouse_press (
                    p + coord.now.origin,
                    button, down);
        }
    };
} 