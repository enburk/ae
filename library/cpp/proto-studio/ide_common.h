#pragma once
#include "gui_widget_button.h"
#include "gui_widget_scroller.h"
namespace ide
{
    struct button : gui::widget<button>
    {
        gui::button sensor;

    };

    template<gui::orientation> struct scroller : gui::scroller<gui::orientation>
    {
        runner.canvas.color = pix::gray;
    };
}
