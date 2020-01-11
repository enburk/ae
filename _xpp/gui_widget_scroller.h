#pragma once
#include "gui_widget.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
namespace gui
{
    template<orientation>
    struct scroller;template<>
    struct scroller<vertical>:
    widget<scroller<vertical>>
    {
        frame runner;
        canvas ground;
        button up, down, page_up, page_down;
        property<double> ratio = 1.6180339887498948482;
        property<int> span = 0, top = 0, step = 1;
        bool touch = false;
        XY touch_point;

        scroller ()
        {
            //up.text = u"";
            //donw.text = u"";
            up.repeating = true;
            down.repeating = true;
        }

        void on_change (void* what) override
        {
            if (what == &span  || what == &top) refresh();
            if (what == &coord || what == &ratio)
            {
                int w = coord.now.size.x;
                int h = coord.now.size.y;
                int d = clamp<int>(std::round(w/ratio.now));
                up.coord = XYWH(0,0,w,d);
                down.coord = XYWH(0,h-d,w,d);
                runner.coord = XYWH(0,d,w,h-d-d);
                refresh();
            }
        }

        void refresh ()
        {
            assert(top.now >= 0);
            assert(span.now >= 0);
            assert(top.now <= span.now);
            int real_page = coord.now.size.y; if (real_page <= 0) return;
            int fake_span = ground.coord.now.size.y;
            int fake_page = fake_span * real_page / max(1, span.now);
            int fake_top  = fake_span * top.now   / max(1, span.now);
            fake_page = min(fake_span, fake_page);
            int w = up.coord.now.size.x;
            int d = up.coord.now.size.y;
            runner.coord = XYWH(0, d+fake_top, w, fake_page);
            page_up.coord = XYWH(0, 0, w, runner.coord.now.y);
            page_down.coord = XYXY(0, d+fake_top+fake_page, w, down.coord.now.y);
            down.enabled = top.now < span.now - real_page;
            up.enabled = top.now > 0;
        }

        void go (int y) { notify (max(0, min (y, span.now - coord.now.size.y))); refresh (); }

        void on_notify (gui::base::widget* w) override
        {
            if (w == &up) go (top.now - step.now);
            if (w == &down) go (top.now + step.now);
            if (w == &page_up) go (top.now - coord.now.size.y);
            if (w == &page_down) go (top.now + coord.now.size.y);
        }

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button != 'L') return;
            if (down && !touch) touch_point = p;
            touch = down; if (!touch) return;
        }
        void on_mouse_hover (XY p) override
        {
            int real_page = coord.now.size.y; if (real_page <= 0) return;
            int fake_span = ground.coord.now.size.y;
            int fake_page = fake_span * real_page / max(1, span.now);
            fake_page = min(fake_span, fake_page); if (fake_page <= 0) return;
            go (top.now - (p.y - touch_point.y) * real_page/fake_page);
        }
    };

    template<>
    struct scroller<horizontal>:
    widget<scroller<horizontal>>
    {
        frame runner;
        canvas ground;
        button up, down, page_up, page_down;
        property<double> ratio = 1.6180339887498948482;
        property<int> span = 0, top = 0, step = 1;
        bool touch = false;
        XY touch_point;

        void refresh ()
        {
        }

        void on_change (void* what) override
        {
        }
    };
}
