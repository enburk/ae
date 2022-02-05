#pragma once
#include "pix_text.h"
namespace doc::view
{
    using pix::XY;
    using pix::text::style;
    using pix::text::style_index;

    struct format
    {
        int width  = max<int>();
        int height = max<int>();

        int columns = 1;
        int gutter  = 0;

        XY alignment = XY{pix::center, pix::center};

        int lpadding = 0;
        int rpadding = 0;

        array<XY> lwrap;
        array<XY> rwrap;

        bool wordwrap = true;
        bool ellipsis = false;

        bool operator == (format const&) const = default;
        bool operator != (format const&) const = default;
    };

    struct token
    {
        str text;
        style_index style;
        str info, link;
        
        bool operator != (token const&) const = default;
        bool operator == (token const&) const = default;
    };

    struct line
    {
        format format;
        int indent = 0;
        int number = -1;
        bool modified = true;
        array<token> tokens;

        bool operator != (line const&) const = default;
        bool operator == (line const&) const = default;
    };

    struct model : polymorphic
    {
        //virtual generator<line> lines (style s, format f) = 0;

        array<line> lines;
        virtual void set (style s, format f) = 0;
    };
}


  