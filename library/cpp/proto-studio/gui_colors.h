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
    inline std::map<str, schema> skins;

    const inline array<const char*> palettes_datae = // https://material.io/design/color/#tools-for-picking-colors
    {                   // 0     // 1     // 2      // 3    // 4     // 5     // 6     // 7     // 8     // 9
        "gray",        "FAFAFA","F5F5F5","EEEEEE","E0E0E0","BDBDBD","9E9E9E","757575","616161","424242","212121", 
                       "000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    //  "light green",
    //  "deep orange",
    };

    void init ()
    {
        metrics::text::height = sys::metrics(sys::font{"Arial",-8}).height;
        metrics::line::width = max (1, metrics::text::height/16);

        int i = 0; str name;
        for (auto s : palettes_datae) {
            RGBA* color = nullptr;
            if (i%21 == 0) name = s; else
            if (i%21 < 11) color = &palettes[name][i%21- 1].back_color; else
            if (i%21 < 21) color = &palettes[name][i%21-11].fore_color;
            if (color) { *color = pix::ARGB(std::strtoul(s, nullptr, 16)); color->a = 255; }
            i++;
        }

        for (auto [name, palette] : palettes) {
            auto & skin = skins[name];

            skin.black   = palette[0].fore_color;
            skin.white   = palette[9].fore_color;

            skin.light   = palette[1];
            skin.normal  = palette[3];
            skin.heavy   = palette[5];

            skin.hovered = palette[6];
            skin.active  = palette[7];
            skin.touched = palette[8];

            skin.disabled.back_color = palette[3].back_color;
            skin.disabled.fore_color = palette[5].back_color;
        }

        for (auto & [name, skin] : skins) {
            skin.error.back_color = pix::ARGB(0xFFB00020);
            skin.error.fore_color = pix::ARGB(0xFFFFFFFF);
            skin.focus.back_color = pix::ARGB(0xFFFFA000);
            skin.focus.fore_color = pix::ARGB(0xFF000000);
        }

        skins[""] = skins["gray"];
    }
}