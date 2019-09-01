#pragma once
#include "pix.h"

namespace sys
{
    namespace window
    {
        inline IMAGE<XRGB> image;
        void update ();

        namespace on
        {
            void resize ();
            void redraw (XYXY);
        }
    }

    namespace mouse
    {
        void cursor(str);

        namespace on
        {
            void move       (XY);
            void wheel      (XY, int);
            void press      (XY, char, bool);
            void clickclick (XY, char);
        }
    }

    namespace font
    {
        FONT::METRICS metrics (FONT);
        GLYPH<XRGB> glyph (FONT, str);
        void render (GLYPH<XRGB> &, bool blend);
    }
}