#pragma once
#include "gui_widget_button.h"
namespace gui
{
    struct list1D : widget<list1D>
    {
        widgetarium<button> items;

        void on_change (void* what) override
        {
        }

    };
}
