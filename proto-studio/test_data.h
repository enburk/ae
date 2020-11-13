#pragma once
#include "data.h"
#include "aux_array.h"
#include "aux_string.h"
using namespace aux;

array<str> test_data()
{
    array<str> out;

    tst::array<int> a, b;
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
