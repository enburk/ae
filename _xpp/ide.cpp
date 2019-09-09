#include "gui_window.h"
#include "gui_widget_frame.h"
//#include "ide_test.h"
//#include "ide_edit.h"
//#include "ide_flist.h"
//using namespace ide;
using namespace pix;

struct IDE : gui::widget
{
    gui::frame editor = gui::frame(this);

    IDE() : gui::widget(nullptr)
    {
        gui::window = this;
        color.go(pix::gray);
        editor.color.go(pix::black);
        editor.widget::color.go(pix::white);
    }

    void on_tick () override
    {
        if (coord.was.size() != coord.now.size())
        {
            editor.move_to(XYWH(
                coord.now.w/4, coord.now.h/4,
                coord.now.w/4, coord.now.h/4));
        }
    }

};

IDE ide;



#include "doc_lexica_txt.h"
