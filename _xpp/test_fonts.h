#pragma once
#include "sys.h"
namespace ide::test::fonts
{
    static IMAGE<XRGB> test1 ()
    {
        str digit = "0123456789";
        str Latin = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        str latin = "abcdefghijklmnopqrstuvwxyz";
        str alnum = Latin + latin + digit;

        array<GLYPH<XRGB>> glyphs;
        int gap = 1; int x = gap; int y = gap; int width = 0; 
        array<FONT> fonts = { FONT("Consolas", 12), FONT("Segoe UI", 16), FONT("Tahoma", 24) };

        for (auto font : fonts)
        for (int r=0; r<4; r++, x = gap, y += 2 + gap +
            sys::font::metrics(font).ascent  +
            sys::font::metrics(font).descent +
            sys::font::metrics(font).linegap)
        {
            font.bold   = r == 2 || r == 3;
            font.italic = r == 1 || r == 3;
            for (char c : alnum)
            {
                static bool even = false; even = !even;
                auto glyph = sys::font::glyph(font, str(c));
                glyph.coord.x = x;
                glyph.coord.y = y + (even ? 0 : 1);
                glyphs += glyph;

                width = max(width, x + glyph.frame.size.x);
                x += (int)std::ceil(glyph.advance) + gap;
            }
        }

        IMAGE<XRGB> image (XY(width, y), XRGB::red);

        for (auto glyph : glyphs)
        {
            glyph.frame = image.frame(XYWH(glyph.coord.x, glyph.coord.y, glyph.frame.size.x, glyph.frame.size.y));
            glyph.fore = XRGB::black;
            glyph.back = XRGB::white;
            sys::font::render(glyph, false);
        }

        return image;
    }

    static IMAGE<XRGB> test2 ()
    {
        str s = "The quick brown fox jumps over the lazy dog";

        array<GLYPH<XRGB>> glyphs;
        int gap = 1; int x = gap; int y = gap; int width = 0; 
        array<FONT> fonts = { FONT("Consolas", 12), FONT("Segoe UI", 14), FONT("Tahoma", 16) };

        for (auto font : fonts)
        for (int r=0; r<4; r++, x = gap, y += 1 + gap +
            sys::font::metrics(font).ascent  +
            sys::font::metrics(font).descent +
            sys::font::metrics(font).linegap)
        {
            font.bold   = r == 2 || r == 3;
            font.italic = r == 1 || r == 3;
            for (char c : s)
            {
                auto glyph = sys::font::glyph(font, str(c));
                glyph.coord.x = x;
                glyph.coord.y = y;
                glyphs += glyph;

                width = max(width, x + glyph.frame.size.x);
                x += (int)std::ceil(glyph.advance);
            }

            x = gap; y += 1 + gap +
            sys::font::metrics(font).ascent  +
            sys::font::metrics(font).descent +
            sys::font::metrics(font).linegap;
            {
                auto glyph = sys::font::glyph(font, s);
                glyph.coord.x = x;
                glyph.coord.y = y;
                glyphs += glyph;

                width = max(width, x + glyph.frame.size.x);
                x += (int)std::ceil(glyph.advance);
            }
        }

        IMAGE<XRGB> image (XY(width, y), XRGB::red);

        for (auto glyph : glyphs)
        {
            glyph.frame = image.frame(XYWH(glyph.coord.x, glyph.coord.y, glyph.frame.size.x, glyph.frame.size.y));
            glyph.fore = XRGB::black;
            glyph.back = XRGB::white;
            sys::font::render(glyph, false);
        }

        return image;
    }

    static IMAGE<XRGB> test ()
    {
        IMAGE<XRGB> image1 = test1();
        IMAGE<XRGB> image2 = test2();
        IMAGE<XRGB> image (XY(max(image1.size.x, image2.size.x), image1.size.y + image2.size.y), XRGB::red);
        image1.copy_to(image);
        image2.copy_to(image.frame(XY(0,image1.size.y)));
        return image;
    }
}
