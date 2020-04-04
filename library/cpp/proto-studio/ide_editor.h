#pragma once
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget_button.h"
#include "gui_widget_scroller.h"
#include "gui_widget_text_editor.h"
using namespace pix;

struct Editor : gui::widget<Editor>
{
    str filename;

    gui::text::editor editor;
    gui::scroller<gui::vertical> vscroller;
    gui::scroller<gui::horizontal> hscroller;

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int h = gui::metrics::text::height;

            editor.background.color = pix::white;
            editor.view.style = sys::glyph_style{
                sys::font{"Consolas", h*110/100},
                pix::black };

            refresh();
        }
    }

    void refresh ()
    {
        auto r = coord.now.local();
        
        int d = gui::metrics::text::height + 2*gui::metrics::line::width;

        if (editor.model.size.x > r.w)
        {
            hscroller.coord = XYWH(r.x,       r.y+r.h-d, r.w-d,     d);
            vscroller.coord = XYWH(r.x+r.w-d, r.y,           d, r.h-d);
            editor   .coord = XYWH(r.x,       r.y,       r.w-d, r.h-d);
        }
        else
        {
            hscroller.coord = XYWH();
            vscroller.coord = XYWH(r.x+r.w-d, r.y,     d, r.h);
            editor   .coord = XYWH(r.x,       r.y, r.w-d, r.h);
        }

        hscroller.span = editor.model.size.x;
        vscroller.span = editor.model.size.y;
        hscroller.top  = editor.offset.now.x;
        vscroller.top  = editor.offset.now.y;
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }

    void on_notify (gui::base::widget* w) override
    {
        if (w == &editor) refresh();
    }
    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &hscroller) editor.offset = XY(n, editor.offset.now.y);
        if (w == &vscroller) editor.offset = XY(editor.offset.now.x, n);
    }
};
