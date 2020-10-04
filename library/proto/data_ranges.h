#pragma once
#include "data_iterators.h"

template<class x>
struct range
{
    typedef forward_iterator<x> iterator;
    struct sentinel;

    auto begin () -> iterator;
    auto end   () -> sentinel;
};
