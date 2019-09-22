#pragma once
#include "pix.h"
#include "gui_widget.h"
namespace gui::text
{
    struct glyph : widget<glyph>
    {
        glyph (base::widget* parent = nullptr) : widget (parent) {}

        GLYPH<RGBA> g;
    };
} 