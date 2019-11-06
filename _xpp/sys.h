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

    struct font
    {
        str face; int size; bool bold; bool italic;

        font(str face = "", int size = 0, bool b=false, bool i=false)
            : face(face), size(size), bold(b), italic(i) {}

        friend bool operator == ( const font & f1, const font & f2 ) { return ! (f1 != f2); }
        friend bool operator != ( const font & f1, const font & f2 ) {
            if (f1.face   != f2.face  ) return true;
            if (f1.size   != f2.size  ) return true;
            if (f1.bold   != f2.bold  ) return true;
            if (f1.italic != f2.italic) return true; return false;
        }
        friend bool operator <  ( const font & f1, const font & f2 ) {
            if (f1.face   <  f2.face  ) return true; if (f1.face   > f2.face  ) return false;
            if (f1.size   <  f2.size  ) return true; if (f1.size   > f2.size  ) return false;
            if (f1.bold   <  f2.bold  ) return true; if (f1.bold   > f2.bold  ) return false;
            if (f1.italic <  f2.italic) return true; if (f1.italic > f2.italic) return false; return false;
        }
        struct metrics
        {
            int height;  // ascent + descent
            int ascent;  // units above the base line
            int descent; // units below the base line (positive value)
            int linegap; // baseline-to-baseline distance = ascent + descent + linegap
            int average_char_width; // usually the width of 'x'
            int maximum_char_width;
            int minimum_char_width;
        };
    };

    struct glyph_style
    {
        struct line { str style; int width = 0; RGBA color;
        friend bool operator == ( const line & l, const line & r ) { return ! (l != r); }
        friend bool operator != ( const line & l, const line & r ) {
            if (l.style != r.style) return true;
            if (l.width != r.width) return true;
            if (l.color != r.color) return true; return false;
        }};

        font font;
        RGBA color;
        RGBA background;
        line underline;  // "solid", "double", "dahsed", "dotted", "wavy"
        line strikeout;
        line outline;

        friend bool operator == ( const glyph_style & l, const glyph_style & r ) { return ! (l != r); }
        friend bool operator != ( const glyph_style & l, const glyph_style & r ) {
            if (l.font       != r.font       ) return true;
            if (l.color      != r.color      ) return true;
            if (l.background != r.background ) return true;
            if (l.underline  != r.underline  ) return true;
            if (l.strikeout  != r.strikeout  ) return true;
            if (l.outline    != r.outline    ) return true; return false;
        }
    };

    struct glyph : glyph_style
    {
        str text;
        XY  size;        // size.y = ascent + descent
        int ascent  = 0; // units above the base line
        int descent = 0; // units below the base line (positive value)
        int advance = 0; // the horizontal distance the pen position must be incremented 

        explicit glyph () = default;
        explicit glyph (str, glyph_style);

        friend bool operator == ( const glyph & l, const glyph & r ) { return ! (l != r); }
        friend bool operator != ( const glyph & l, const glyph & r ) {
            if (l.text       != r.text       ) return true;
            if (l.size       != r.size       ) return true;
            if (l.ascent     != r.ascent     ) return true;
            if (l.descent    != r.descent    ) return true;
            if (l.advance    != r.advance    ) return true;
            if (l.font       != r.font       ) return true;
            if (l.color      != r.color      ) return true;
            if (l.background != r.background ) return true;
            if (l.underline  != r.underline  ) return true;
            if (l.strikeout  != r.strikeout  ) return true;
            if (l.outline    != r.outline    ) return true; return false;
        }
    };

    struct token
    {
        array<glyph> glyphs;

        explicit token (str, glyph_style);
        explicit token () = default;
    };

    font::metrics metrics (font);
    void render (glyph, Frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
    void render (token, Frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
}