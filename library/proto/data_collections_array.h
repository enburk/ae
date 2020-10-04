#pragma once
#include <vector>
#include "data_collections.h"

namespace tst
{
template<class x>//, class C = std::vector<x>>
struct array : contiguous_collection<x>
{
    std::vector<x> data;

    typedef contiguous_collection<x> base;
    using typename base::iterator;
    using typename base::sentinel;

    auto size () const { return (int) data.size(); }
    void size (int n) { data.resize(n); }

    auto begin () { return iterator{.pointer=data.data()}; }
    auto end   () { return sentinel{begin() + size ()}; }

    void operator += (x const&  element) { data.push_back(element); }
    void operator += (x tsnoc&& element) { data.push_back(element); }

};
}
