#pragma once
#include "data.h"
#include "../cpp/aux_array.h"
#include "../cpp/aux_string.h"
using namespace aux;

array<str> test_data()
{
    array<str> out;

    tst::array<int> a, b;
    a += 1;
    a += 2;
    a += 3;
    for (auto x : a)
        b += x;

    str s;
    for (auto x : a)
        s += std::to_string(x);

    out += s;
    return out;
}
