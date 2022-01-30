#pragma once
#include "pix_text.h"
namespace doc::view
{
    using pix::XY;
    using pix::text::style;
    using pix::text::style_index;

    struct padding
    {
        int global = 0; array<XY> bars;

        void operator  = (int width) { global  = width; }
        void operator += (int width) { global += width; }
        void operator -= (int width) { global -= width; }

        int max (int height)
        {
            int d = 0;
            for (auto bar : bars) {
                d = aux::max(d, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break; }
            return d + global;
        }

        void skip (int height)
        {
            while (not bars.empty()) {
                auto& bar = bars.front();
                bar.y -= height;
                if (bar.y >= 0)
                    break;
                height = -bar.y;
                bars.erase(0);
            }
        }

        bool operator == (padding const&) const = default;
        bool operator != (padding const&) const = default;
    };

    struct format
    {
        int width  = max<int>();
        int height = max<int>();

        int columns = 1;
        int gutter = -1;

        XY alignment = XY{pix::center, pix::center};

        padding lpadding;
        padding rpadding;

        bool wordwrap = true;
        bool ellipsis = false;

        bool operator == (const format & f) const = default;
        bool operator != (const format & f) const = default;
    };

    struct token
    {
        str text;
        style_index style;
        str info, link;
        
        bool operator != (const token & d) const = default;
        bool operator == (const token & d) const = default;
    };

    struct line
    {
        format format;
        int indent = 0;
        array<token> tokens;

        bool operator != (const line & d) const = default;
        bool operator == (const line & d) const = default;
    };

    struct model : polymorphic
    {
        //virtual generator<line> lines (style s, format f) = 0;

        array<line> lines;
        virtual void set (style s, format f) = 0;
    };
}


  