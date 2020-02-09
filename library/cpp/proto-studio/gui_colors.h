#pragma once
#include <map>
#include "../sys.h"
namespace gui
{
    using namespace pix;

    namespace metrics
    {
        namespace text { inline int height; };
        namespace line { inline int width;  };
    };

    struct pair_of_colors { RGBA fore_color, back_color; };

    inline std::map<str, pair_of_colors[10]> palettes;

    struct schema
    {
        sys::font font;
        RGBA black, white;
        pair_of_colors light, normal, heavy,
        active, hovered, touched, disabled,
        focus, error;
    };
    inline std::map<str, schema> schemas;

    const inline array<const char*> palettes_datae = // https://material.io/design/color/#tools-for-picking-colors
    {                   // 0     // 1     // 2      // 3    // 4     // 5     // 6     // 7     // 8     // 9
        "gray",        "FAFAFA","F5F5F5","EEEEEE","E0E0E0","BDBDBD","9E9E9E","757575","616161","424242","212121", 
                       "000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    //  "light green",
    //  "deep orange",
    };

    void init ()
    {
        metrics::text::height = sys::screen::size.y/90;
        metrics::line::width = max (1, metrics::text::height/16);

        int i = 0; str name;
        for (auto s : palettes_datae) {
            RGBA* color = nullptr;
            if (i%21 == 0) name = s; else
            if (i%21 < 11) color = &palettes[name][i%21- 1].back_color; else
            if (i%21 < 21) color = &palettes[name][i%21-11].fore_color;
            if (color) { *color = ARGB(std::strtoul(s, nullptr, 16)); color->a = 255; }
            i++;
        }

        for (auto [name, palette] : palettes) {
            auto & schema = schemas[name];

            schema.black   = palette[0].fore_color;
            schema.white   = palette[9].fore_color;

            schema.light   = palette[1];
            schema.normal  = palette[3];
            schema.heavy   = palette[5];

            schema.hovered = palette[6];
            schema.active  = palette[7];
            schema.touched = palette[8];

            schema.disabled.back_color = palette[3].back_color;
            schema.disabled.fore_color = palette[5].back_color;
        }

        for (auto & [name, schema] : schemas) {
            schema.error.back_color = ARGB(0xFFB00020);
            schema.error.fore_color = ARGB(0xFFFFFFFF);
            schema.focus.back_color = ARGB(0xFFFFA000);
            schema.focus.fore_color = ARGB(0xFF000000);
        }

        schemas[""] = schemas["gray"];
    }
}