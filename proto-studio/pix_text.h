#pragma once
#include "aux_string.h"
#include "pix_color.h"
#include "pix_image.h"
namespace pix
{
    using namespace aux;

    struct font
    {
        str face; int size; bool bold; bool italic;

        bool operator != (const font & f) const { return ! (*this == f); }
        bool operator == (const font & f) const { return
             face   == f.face &&
             size   == f.size &&
             bold   == f.bold &&
             italic == f.italic; }

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

    font::metrics metrics (font);

    struct glyph_style
    {
        struct line { str style; int width = 0; RGBA color;
        bool operator != (const line & l) const { return ! (*this == l); }
        bool operator == (const line & l) const { return
             style == l.style &&
             width == l.width &&
             color == l.color; }
        };

        font font;
        RGBA color;
        RGBA background;
        line underline;  // "solid", "double", "dahsed", "dotted", "wavy"
        line strikeout;
        line outline;

        bool operator != (const glyph_style & s) const { return ! (*this == s); }
        bool operator == (const glyph_style & s) const { return
             font       == s.font       &&
             color      == s.color      &&
             background == s.background &&
             underline  == s.underline  &&
             strikeout  == s.strikeout  &&
             outline    == s.outline; }
    };

    struct glyph_style_index
    {
        int value = 0;

        bool operator != (glyph_style_index i) const { return value != i.value; }
        bool operator == (glyph_style_index i) const { return value == i.value; }
        bool operator <  (glyph_style_index i) const { return value <  i.value; }

        static inline array<glyph_style> glyph_styles = {glyph_style{}};

        const glyph_style & style () const { return glyph_styles[value]; }
        /***/ glyph_style & style () /***/ { return glyph_styles[value]; }

        explicit glyph_style_index () = default;
        explicit glyph_style_index (const glyph_style & style) :
            value ((int)(glyph_styles.find_or_emplace(style) -
                         glyph_styles.begin())) {}
    };

    struct glyph_metrics
    {
        int  width   = 0; // units
        int  ascent  = 0; // units above the base line
        int  descent = 0; // units below the base line (positive value)
        int  advance = 0; // the pen position increment = width + advance
        XYWH outlines;    // boundaries of the actual image

        bool operator != (const glyph_metrics & m) const { return ! (*this == m); }
        bool operator == (const glyph_metrics & m) const { return
             width   == m.width   &&
             ascent  == m.ascent  &&
             descent == m.descent &&
             advance == m.advance; }
    };

    struct glyph : glyph_metrics
    {
        str text; glyph_style_index style_index;

        explicit glyph () = default;
        explicit glyph (str text, glyph_style_index);
        explicit glyph (str text, glyph_style style):
            glyph (text, glyph_style_index(style)) {}

        bool operator != (const glyph & g) const { return ! (*this == g); }
        bool operator == (const glyph & g) const { return text == g.text &&
            style_index == g.style_index; }

        glyph_style style () const { return style_index.style(); }

        void render (pix::frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
    };
}
