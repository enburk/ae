#pragma once
#include "gui_colors.h"
#include "gui_widget_image.h"
#include "gui_widget_canvas.h"
using namespace pix;

struct TestFont1 : gui::widget<TestFont1>
{
    gui::image gui_image; pix::image<RGBA> image;
    
    void on_change (void* what) override
    {
        if (what != & alpha || alpha.to == 0 ||
            coord.now.size == image.size ||
            coord.now.size == XY()) return;

        image.resize(coord.now.size); image.fill(pix::red);
        gui_image.coord = coord.now.local();
        gui_image.source = image;

        str digit = "0123456789";
        str Latin = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        str latin = "abcdefghijklmnopqrstuvwxyz";
        str alnum = Latin + latin + digit;
        array<
        sys::font> fonts = {
        sys::font("Consolas", gui::metrics::text::height*1/1),
        sys::font("Arial",    gui::metrics::text::height*3/2),
        sys::font("Segoe UI", gui::metrics::text::height*4/2),
        sys::font("Tahoma",   gui::metrics::text::height*5/2)};

        int gap = 1; int x = gap; int y = gap;

        for (auto font : fonts)
        for (int r=0; r<4; r++, x = gap, y += gap +
            sys::metrics(font).ascent  +
            sys::metrics(font).descent +
            sys::metrics(font).linegap)
        {
            sys::glyph_style style;
            style.color = pix::black;
            style.color.a = 230;
            style.background = pix::white;
            style.background.a = 230;
            style.font = font;
            style.font.bold   = r == 2 || r == 3;
            style.font.italic = r == 1 || r == 3;

            for (char c : alnum) {
                auto glyph = sys::glyph(str(c), style);
                sys::render(glyph, image.crop(XYWH(x, y, glyph.size.x, glyph.size.y)), XY(), 230, x);
                x += glyph.size.x + glyph.advance + gap;
            }
        }
    }
};

struct TestFont2 : gui::widget<TestFont2>
{
    gui::image gui_image; pix::image<RGBA> image;
    
    void on_change (void* what) override
    {
        if (what != & alpha || alpha.to == 0 ||
            coord.now.size == image.size ||
            coord.now.size == XY()) return;

        image.resize(coord.now.size); image.fill(pix::red);
        gui_image.coord = coord.now.local();
        gui_image.source = image;

        str s = "The quick brown fox jumps over the lazy dog";
        array<
        sys::font> fonts = {
        sys::font("Consolas", gui::metrics::text::height*1/1),
        sys::font("Arial",    gui::metrics::text::height*3/2),
        sys::font("Segoe UI", gui::metrics::text::height*4/2),
        sys::font("Tahoma",   gui::metrics::text::height*4/2)};

        int gap = 1; int x = gap; int y = gap;

        for (auto font : fonts)
        for (int r=0; r<4; r++, x = gap, y += gap +
            sys::metrics(font).ascent  +
            sys::metrics(font).descent +
            sys::metrics(font).linegap)
        {
            sys::glyph_style style;
            style.color = pix::black;
            style.color.a = 230;
            style.background = pix::white;
            style.background.a = 230;
            style.font = font;
            style.font.bold   = r == 2 || r == 3;
            style.font.italic = r == 1 || r == 3;

            for (char c : s) {
                auto glyph = sys::glyph(str(c), style);
                sys::render(glyph, image.crop(XYWH(x, y, glyph.size.x, glyph.size.y)), XY(), 230, x);
                x += glyph.size.x + glyph.advance;
            }
            x = gap; y += gap +
            sys::metrics(font).ascent  +
            sys::metrics(font).descent +
            sys::metrics(font).linegap;
            {
                auto glyph = sys::glyph(s, style);
                sys::render(glyph, image.crop(XYWH(x, y, glyph.size.x, glyph.size.y)), XY(), 230, x);
                x += glyph.size.x + glyph.advance;
            }
        }
    }
};

struct TestFont3 : gui::widget<TestFont3>
{
    gui::text::glyph linea;
    gui::text::token lineb;
    gui::text::line  linec;
    gui::widgetarium<gui::text::line> lines;
    
    void on_change (void* what) override
    {
        if (what != & alpha || alpha.to == 0 || lines.size () > 0 ||
            coord.now.size == XY()) return;

        lines.resize(coord.now.size);

        str s = "The quick brown fox jumps over the lazy dog";
        array<
        sys::font> fonts = {
        sys::font("Consolas", gui::metrics::text::height*1/1),
        sys::font("Arial",    gui::metrics::text::height*3/2),
        sys::font("Segoe UI", gui::metrics::text::height*4/2),
        sys::font("Tahoma",   gui::metrics::text::height*5/2)};

        int gap = 1; int x = gap; int y = gap;

        sys::glyph_style style;
        style.color = pix::black;
        style.color.a = 230; // style.background = background;
        style.font = fonts[0];

        linea.Glyph = sys::glyph(s, style);
        linea.move_to(XY(x, y));
        y += linea.coord.now.size.y;

        lineb.Token = sys::token(s, style);
        lineb.move_to(XY(x, y));
        y += lineb.coord.now.size.y;

        linec.fill(s, style);
        linec.move_to(XY(x, y));
        y += linec.coord.now.size.y;

        for (auto font : fonts)
        for (int r=0; r<4; r++)
        {
            RGBA 
            background = pix::white;
            background.a = 192;

            style.font = font;
            style.font.bold   = r == 2 || r == 3;
            style.font.italic = r == 1 || r == 3;

            //auto & line1 = lines.emplace_back();
            //line1.fill(s, style);
            //line1.move_to(XY(x, y));
            //line1.canvases.emplace_back().color = background;
            //line1.canvases.back().resize(line1.coord.now.size);
            //y += line1.coord.now.size.y;
            //
            //auto & line2 = lines.emplace_back();
            //line2.append(sys::token(s, style));
            //line2.resize(line2.glyphs.coord.now.size);
            //line2.move_to(XY(x, y));
            //line2.canvases.emplace_back().color = background;
            //line2.canvases.back().resize(line2.coord.now.size);
            //y += line2.coord.now.size.y;
        }
    }
};
