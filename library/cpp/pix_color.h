#pragma once
#include <cstdint>
#include <functional>
#include "aux_abc.h"
namespace pix
{
    struct RGBA
    {
        union
        {
            struct {
            uint8_t b, g, r, a; };
            uint8_t channels [4];
            uint32_t value; // byte-order-dependent
        };

        RGBA () : value (0) {}
        RGBA (int r, int g, int b, int a = 255) : b(b), g(g), r(r), a(a) {}

        bool operator == ( const RGBA & c ) const { return value == c.value; }
        bool operator != ( const RGBA & c ) const { return value != c.value; }
        bool operator <  ( const RGBA & c ) const { return value <  c.value; }

        void blend (RGBA c, uint8_t alpha = 255) {
            uint8_t
                A = alpha == 255 ? c.a : c.a * alpha >> 8;
            r = A == 255 ? c.r : r + ( A * (c.r - r) >> 8 );
            g = A == 255 ? c.g : g + ( A * (c.g - g) >> 8 );
            b = A == 255 ? c.b : b + ( A * (c.b - b) >> 8 );
        }

        static RGBA random(uint8_t l = 64, uint8_t u = 255) { return RGBA(
               aux::random<int>(l, u),
               aux::random<int>(l, u),
               aux::random<int>(l, u)); }
    };

    inline RGBA ARGB (uint32_t value)
    {
        RGBA c;
        c.a = (value >> 24) & 0xFF;
        c.r = (value >> 16) & 0xFF;
        c.g = (value >>  8) & 0xFF;
        c.b = (value      ) & 0xFF;
        return c;
    }

    const RGBA
    black  = ARGB(0xFF000000), silver = ARGB(0xFFC0C0C0), gray    = ARGB(0xFF808080), white   = ARGB(0xFFFFFFFF),
    maroon = ARGB(0xFF800000), red    = ARGB(0xFFFF0000), purple  = ARGB(0xFF800080), fuchsia = ARGB(0xFFFF00FF),
    green  = ARGB(0xFF008000), lime   = ARGB(0xFF00FF00), olive   = ARGB(0xFF808000), yellow  = ARGB(0xFFFFFF00),
    navy   = ARGB(0xFF000080), blue   = ARGB(0xFF0000FF), teal    = ARGB(0xFF008080), aqua    = ARGB(0xFF00FFFF);

    struct  MONO
    {
        union { uint8_t value; uint8_t channels [1]; };

        MONO () : value (0) {}
        MONO (int value) : value(value) {}
        MONO (RGBA c) { value = aux::clamp<uint8_t>(255 * (0.212671*c.r + 0.715160*c.g + 0.072169*c.b)); }

        bool operator == (const MONO & c) const { return value == c.value; }
        bool operator != (const MONO & c) const { return value != c.value; }
        bool operator <  (const MONO & c) const { return value <  c.value; }
    };
}

namespace std
{
    template <> struct hash<pix::RGBA>
    {
        std::size_t operator()(const pix::RGBA& c) const
        {
            return std::hash<std::uint32_t>()(c.value);
        }
    };
}

