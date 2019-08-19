#pragma once
#include <vector>
#include "pix.h"

namespace sys
{
    namespace window
    {
        inline IMAGE<XRGB> image;
        void update (XYXY);

        namespace on
        {
            void resize ();
            void redraw (XYXY);
        }
    }

    namespace font
    {
        FONT::METRICS metrics (FONT);
        GLYPH<XRGB> glyph (FONT, str);
        void render (GLYPH<XRGB> &);
    }
}