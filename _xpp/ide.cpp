#include "gui_window.h"
#include "gui_widget_image.h"
#include "gui_widget_canvas.h"
#include "gui_widget_button.h"
#include "ide_console.h"
#include "ide_flist.h"
#include "ide_editor.h"
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

    struct document
    {
        doc::Document document;
        //gui::text::editor::model::context context;
    };

    std::map<std::filesystem::path, document> documents;

    IDE()
    {
        gui::window = this;
        button_test.text.text = "Test";
        button_test.kind = gui::button::toggle;
        test.hide();
    }

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            gui::schemas[""].font = sys::font{"Segoe UI", gui::metrics::text::height};

            toolbar.color = gui::schemas[""].light.back_color;

            int W = coord.now.w; int w = gui::metrics::text::height*10;
            int H = coord.now.h; int h = gui::metrics::text::height*2;

            toolbar.coord = XYWH(0, 0, W, h);
            button_test.coord = XYWH(W-w, 0, w, h);

            test.coord = XYWH(W*0/4, h, W*4/4, H-h);
            flist.coord = XYWH(W*0/4, h, W*1/4, H-h);
            editor.coord = XYWH(W*1/4, h, W*2/4, H-h);
            console.coord = XYWH(W*3/4, h, W*1/4, H-h);
        }
    }

    void on_notify (gui::base::widget* w) override
    {
        if (w == &button_test) test.show (test.alpha.to == 0, gui::time(500));

        if (w == &flist)
        {
            //documents[flist.path.was].context= editor.model.context;
            //editor.load(flist.path);
        }
    }

    void on_focus (bool on) override { editor.on_focus(on); }
    void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }
    void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
};

IDE ide;



