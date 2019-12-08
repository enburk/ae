#pragma once
#include "aux_string.h"
#include "pix_color.h"
#include "pix_coord.h"
namespace pix
{
    struct font
    {
        str face; int size; bool bold; bool italic;

        font(str face = "", int size = 0, bool b=false, bool i=false)
            : face(face), size(size), bold(b), italic(i) {}

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
            font      == s.font       &&
            color     == s.color      &&
            background== s.background &&
            underline == s.underline  &&
            strikeout == s.strikeout  &&
            outline   == s.outline; }
    };

    struct glyph_metrics
    {
        XY  size;        // size.y = ascent + descent
        int ascent  = 0; // units above the base line
        int descent = 0; // units below the base line (positive value)
        int advance = 0; // the pen position increment = size.x + advance

        bool operator != (const glyph_metrics & m) const { return ! (*this == m); }
        bool operator == (const glyph_metrics & m) const { return
            size    == m.size    &&
            ascent  == m.ascent  &&
            descent == m.descent &&
            advance == m.advance; }
    };
}