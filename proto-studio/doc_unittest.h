#pragma once
#include "data.h"
#include "aux_array.h"
#include "aux_string.h"
namespace data
{
    aux::array<aux::str> unittest()
    {
        aux::array<aux::str> out;

        out += "=== array ===";
        out += "=== string ===";

        array<int> a, b;
    //    a += 1;
    //    a += 2;
    //    a += 3;
    //    for (auto x : a)
    //        b += x;

        a = b;
        //b = b + b;
        b += b.from(1);

        b.remove_if([](auto i){ return (i%2) == 0; });

        str s;
        for (auto x : b)
            s += std::to_string(x);

        out += s;
        return out;
    }
}
