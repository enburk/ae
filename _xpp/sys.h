#pragma once
#include "pix.h"
namespace sys
{
    namespace screen
    {
        using namespace pix;
        inline XY size;
    }

    namespace window
    {
        using namespace pix;
        inline Image<RGBA> image;
        void update ();
        void timing ();
        namespace on
        {
            void start  ();
            void finish ();
            void pause  ();
            void resume ();
            void timing ();
            void resize ();
        }
    }

    namespace mouse
    {
        using namespace pix;
        void cursor(str);
        namespace on
        {
            void move  (XY);
            void wheel (XY, int);
            void press (XY, char, bool);
            void leave ();
        }
    }

    namespace font
    {
        using namespace pix;
        FONT::METRICS metrics (FONT);
        GLYPH<RGBA> glyph (FONT, str);
        void render (GLYPH<RGBA> &, bool blend);
    }
}