#pragma once
#include "pix_coord.h"
#include "pix_image.h"
#include "pix_typo_font.h"

struct  XRGB
{
    union
    {
        struct {
        uint8_t b, g, r, x; };
        uint8_t channels [4];
        uint32_t xrgb;
    };

    XRGB (                          ) : xrgb() {}
    XRGB (uint32_t xrgb             ) : xrgb(xrgb) {}
    XRGB (int r, int g, int b       ) : b(b), g(g), r(r), x(255) {}
    XRGB (int r, int g, int b, int x) : b(b), g(g), r(r), x(x) {}

    bool operator == ( const XRGB & c ) const { return xrgb == c.xrgb; }
    bool operator != ( const XRGB & c ) const { return xrgb != c.xrgb; }
    bool operator <  ( const XRGB & c ) const { return xrgb <  c.xrgb; }

    enum
    {
        black  = 0xFF000000, silver = 0xFFC0C0C0, gray    = 0xFF808080, white   = 0xFFFFFFFF,
        maroon = 0xFF800000, red    = 0xFFFF0000, purple  = 0xFF800080, fuchsia = 0xFFFF00FF,
        green  = 0xFF008000, lime   = 0xFF00FF00, olive   = 0xFF808000, yellow  = 0xFFFFFF00,
        navy   = 0xFF000080, blue   = 0xFF0000FF, teal    = 0xFF008080, aqua    = 0xFF00FFFF,
    };
};
 