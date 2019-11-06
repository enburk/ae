#include "gui_window.h"
#include "gui_widget_image.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "ide_console.h"
#include "ide_flist.h"
#include "ide_edit.h"
#include "test.h"
using namespace pix;

struct IDE : gui::widget<IDE>
{
    gui::canvas toolbar;
    gui::button button_test;

    Console console;
    Editor editor;
    Flist flist;
    Test test;

    IDE()
    {
        gui::window = this;
        toolbar.color = pix::gray;
        test.hide();
    }

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            int W = coord.now.w; int w = W/4;
            int H = coord.now.h; int h = sys::screen::size.y/40;

            toolbar.coord = XYWH(0, 0, W, h);
            button_test.coord = XYWH(W-5*h, 2, 4*h, h-4);

            test.coord = XYWH(w*0, h, W, H-h);
            flist.coord = XYWH(w*0, h, w, H-h);
            editor.coord = XYWH(w*1, h, 2*w, H-h);
            console.coord = XYWH(w*3, h, W-3*w, H-h);
        }
    }

    virtual void on_notify (gui::base::widget* w)
    {
        if (w == &button_test) test.show (test.alpha.to == 0, gui::time(500));
    }
};

IDE ide;



