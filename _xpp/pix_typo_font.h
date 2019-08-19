#pragma once
#include "aux_string.h"

struct FONT
{
    str  face;
    int  size;
    bool bold;
    bool italic;
    bool strike;
    bool underline;

    friend bool operator == ( const FONT & f1, const FONT & f2 ) { return ! ( f1 != f2 ); }
    friend bool operator != ( const FONT & f1, const FONT & f2 ) {
        if (f1.face      != f2.face     ) return true;
        if (f1.size      != f2.size     ) return true;
        if (f1.bold      != f2.bold     ) return true;
        if (f1.italic    != f2.italic   ) return true;
        if (f1.strike    != f2.strike   ) return true;
        if (f1.underline != f2.underline) return true; return false;
    }
    friend bool operator <  ( const FONT & f1, const FONT & f2 ) {
        if (f1.face      <  f2.face     ) return true; if (f1.face      >  f2.face     ) return false;
        if (f1.size      <  f2.size     ) return true; if (f1.size      >  f2.size     ) return false;
        if (f1.bold      <  f2.bold     ) return true; if (f1.bold      >  f2.bold     ) return false;
        if (f1.italic    <  f2.italic   ) return true; if (f1.italic    >  f2.italic   ) return false;
        if (f1.strike    <  f2.strike   ) return true; if (f1.strike    >  f2.strike   ) return false;
        if (f1.underline <  f2.underline) return true; if (f1.underline >  f2.underline) return false; return false;
    }
    struct METRICS
    {
        int  height;   // ascent + descent
        int  ascent;   // units above the base line
        int  descent;  // units below the base line (positive value)
        int  linegap;  // external leading; baseline-to-baseline distance should be computed as: ascent + descent + linegap
        int  avecharw; // average char width (usually width of 'x')
        int  maxcharw; // maximum char width
        bool monospace;
    };
};

template<typename color> struct GLYPH
{
    str text;
    FONT font;
    color fore, back;
    FRAME<color> frame;
    real xbearing = 0; // the horizontal distance from the current pen position to the glyph's left image edge
    real ybearing = 0; // the vertical distance from the baseline to the top of the glyph's image
    real advance  = 0; // the horizontal distance the pen position must be incremented 
};

MAKE_HASHABLE(FONT, t.face, t.size, t.bold, t.italic, t.strike, t.underline);
MAKE_HASHABLE(GLYPH<XRGB>, t.text, t.font, t.fore, t.back);
