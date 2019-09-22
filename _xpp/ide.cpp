#include "gui_window.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "ide_console.h"
#include "ide_flist.h"
#include "ide_edit.h"
using namespace pix;

struct IDE : gui::widget<IDE>
{
    Flist flist;
    Editor editor;
    Console console;

    gui::button button_test;

    gui::canvas test;

    IDE()
    {
        gui::window = this;
        color = pix::gray;
        test.color = pix::red;
        test.hide();
    }

    void on_change () override
    {
        if (coord.was.size() != coord.now.size())
        {
            int W = coord.now.w; int w = W/4;
            int H = coord.now.h; int h = sys::screen::size.y/40;

            flist.move_to(XYWH(w*0, h, w, H-h));
            editor.move_to(XYWH(w*1, h, 2*w, H-h));
            console.move_to(XYWH(w*3, h, W-3*w, H-h));

            //button_test.move_to(XYWH(W-5*h, 2, 4*h, h-4));
            button_test.move_to(XYWH(h, 2, 4*h, h-2));

            if (test.alpha.to == 255) test.move_to(coord.now - coord.now.origin());
        }
    }

    virtual void on_notify (widget* w)
    {
        //if (w == &button_test) test.show();
    }
};

IDE ide;



