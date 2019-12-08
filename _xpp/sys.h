#pragma once
#include "aux_array.h"
#include "aux_string.h"
#include "aux_utils.h"
#include "pix.h"
namespace sys
{
    using pix::XY;
    using pix::XYWH;
    using pix::XYXY;
    using pix::RGBA;
    using pix::Image;
    using pix::Frame;
    using pix::font;
    using pix::glyph_style;
    using pix::glyph_metrics;

    namespace screen
    {
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
            void start   ();
            void finish  ();
            void turn_on ();
            void turn_off();
            void pause   ();
            void resume  ();
            void timing  ();
            void resize  ();
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

    namespace keyboard
    {
        namespace on
        {
            void focus (bool);
            void press (str, bool);
            void input (str);
        }
    }

    inline array<sys::glyph_style> glyph_styles;
    inline int style_index (const glyph_style & style) {
        return glyph_styles.find_or_emplace(style);
    }

    struct glyph : glyph_metrics
    {
        str text; int style_index;

        explicit glyph (str, glyph_style);
        explicit glyph () = default;

        const glyph_style & style () const { return glyph_styles[style_index]; }

        bool operator != (const glyph & g) const { return ! (*this == g); }
        bool operator == (const glyph & g) const { return text == g.text &&
            style_index == g.style_index; }
    };

    struct token : glyph
    {
        array<glyph> glyphs; array<XY> offsets;

        explicit token (str, glyph_style);
        explicit token () = default;
    };

    font::metrics metrics (font);
    void render (glyph, Frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
    void render (token, Frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
}