#pragma once
#include "aux_array.h"
#include "aux_string.h"
namespace doc::lexica
{
    inline constexpr auto ascii (char c) { return c >= ' ' && c <= '~'; };
    inline constexpr auto space (char c) { return c == ' ' || c =='\t'; };
    inline constexpr auto digit (char c) { return c >= '0' && c <= '9'; };
    inline constexpr auto alpha (char c) { return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z'; };
}


 