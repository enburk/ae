#pragma once
#include <map>
#include <variant>
#include "data_struct_string.h"
#include "pix_color.h"
#include "pix_text.h"
namespace gui
{
    using namespace data;
    using namespace pix;

    namespace metrics
    {
        namespace text { inline int height; };
        namespace line { inline int width;  };
    };

    struct schema
    {
        pix::font font;
        std::pair<RGBA,RGBA>
        ultralight, light, normal, middle, heavy, dark,
        active, hovered, touched, focused, disabled,
        highlight, selection, error;
    };
    inline std::map<str, schema> skins;

    inline std::map<str, std::pair<RGBA,RGBA>[10]> palettes;

    const inline array<const char*> palettes_data = {
    // https://material.io/design/color/#tools-for-picking-colors
    //                 0        1        2        3        4        5        6        7        8        9
    "deep orange", "FBE9E7","FFCCBC","FFAB91","FF8A65","FF7043","FF5722","F4511E","E64A19","D84315","BF360C",
                   "000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "brown",       "EFEBE9","D7CCC8","BCAAA4","A1887F","8D6E63","795548","6D4C41","5D4037","4E342E","3E2723",
                   "000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "gray",        "FAFAFA","F5F5F5","EEEEEE","E0E0E0","BDBDBD","9E9E9E","757575","616161","424242","212121", 
                   "000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "blue gray",   "ECEFF1","CFD8DC","B0BEC5","90A4AE","78909C","607D8B","546E7A","455A64","37474F","263238",
                   "000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    };

    void init ()
    {
        metrics::text::height = pix::metrics(pix::font{}).height;
        metrics::line::width = max (1, metrics::text::height/16);

        int i = 0; str name;
        for (auto s : palettes_data) {
            RGBA* color = nullptr;
            if (i%21 == 0) name = s; else
            if (i%21 < 11) color = &palettes[name][i%21- 1].first; else
            if (i%21 < 21) color = &palettes[name][i%21-11].second;
            if (color) { *color = pix::ARGB(std::strtoul(s, nullptr, 16)); color->a = 255; }
            i++;
        }

        for (int i=0; i<10; i++)
            palettes[""][i] = std::pair{
                RGBA::random(), RGBA::random()};

        for (auto [name, palette] : palettes) {
            auto & skin = skins[name];

            skin.ultralight = palette[0];
            skin.light      = palette[1];
            skin.normal     = palette[3];
            skin.middle     = palette[4];
            skin.heavy      = palette[5];
            skin.dark       = palette[9];

            skin.hovered    = palette[6];
            skin.active     = palette[7];
            skin.touched    = palette[8];
            skin.focused    = std::pair{RGBA::amber, RGBA::black};
            skin.disabled   = std::pair{palette[3].first, palette[5].first};

            skin.highlight  = palette[3];
            skin.selection  = palette[4];
            skin.error      = std::pair{RGBA::error, RGBA::white};
        }

        skins[""] = skins["gray"];
    }
    void done () {}
}