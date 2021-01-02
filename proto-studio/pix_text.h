#pragma once
#include "data_struct_string.h"
#include "pix_color.h"
#include "pix_image.h"
namespace pix
{
    using namespace data;

    struct font
    {
        str face; int size; bool bold; bool italic;

        bool operator == (font const&) const = default;
        bool operator != (font const&) const = default;

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

    namespace text
    {
        struct style
        {
            struct line {
            str style; int width = 0; RGBA color;
            bool operator == (line const&) const = default;
            bool operator != (line const&) const = default;
            };

            font font;
            RGBA color;
            line underline;  // "solid", "double", "dahsed", "dotted", "wavy"
            line strikeout;
            line outline;

            bool operator == (style const&) const = default;
            bool operator != (style const&) const = default;
        };

        struct style_index
        {
            int value = 0;

            static inline array<style> styles = {style{}};

            text::style const& style () const { return styles[value]; }
            text::style /***/& style () /***/ { return styles[value]; }

            auto operator <=> (style_index const&) const = default;

            explicit style_index () = default;
            explicit style_index (text::style const& style) :
                value ((int)(styles.find_or_emplace(style) -
                             styles.begin())) {}
        };

        struct metrics
        {
            int  width   = 0; // units
            int  ascent  = 0; // units above the base line
            int  descent = 0; // units below the base line (positive value)
            int  advance = 0; // the pen position increment = width + advance
            XYWH outlines;    // boundaries of the actual image

            bool operator == (metrics const&) const = default;
            bool operator != (metrics const&) const = default;
        };

        struct format
        {
            int width  = max<int>();
            int height = max<int>();

            XY alignment = XY{center, center};

            bool word_wrap = true;
            bool ellipsis = false;

            XY margin_left;
            XY margin_right;

            bool operator == (const format & f) const = default;
            bool operator != (const format & f) const = default;
        };
    }

    struct glyph : text::metrics
    {
        str text; text::style_index style_index;

        explicit glyph () = default;
        explicit glyph (str text, text::style_index);
        explicit glyph (str text, text::style style):
            glyph (text, text::style_index(style)) {}

        text::style style () const { return style_index.style(); }

        bool operator == (glyph const&) const = default;
        bool operator != (glyph const&) const = default;

        void render (pix::frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
    };
}
