#pragma once
#include <vector>
#include "data_collections.h"

namespace tst
{
template<class x>//, class container = std::vector<x>>
struct array : contiguous_collection<x>
{
    typedef std::vector<x> container;

    container data;

    typedef array same;
    typedef range<x> range;
    typedef contiguous_collection<x> base;
    using typename base::iterator;
    using typename base::sentinel;

    auto size () const { return (int) data.size(); }
    void size (int n) { data.resize(n); }

    auto begin () { return iterator{.pointer = data.data() }; }
    auto end   () { return sentinel{ begin() + size() }; }

    void operator += (x const&  element) { data.push_back(element); }
    void operator += (x tsnoc&& element) { data.push_back(element); }

    //collection_impl;
    friend same operator + (same  l, x     r) { same c; c += l; c += r; return c; }
    friend same operator + (same  l, range r) { same c; c += l; c += r; return c; }
    friend same operator + (same  l, same  r) {
        same c;
        c += l;
        c += r;
        return c; }
    friend same operator + (range l, same  r) { same c; c += l; c += r; return c; }
    friend same operator + (x     l, same  r) { same c; c += l; c += r; return c; }
//    void operator += (same && c) { for (auto && e : c) (*this) += e; }
    void operator += (range   r) { for (auto & e : r) (*this) += e; }

    #include "data_ranges_forward.h"
    #include "data_ranges_random.h"
};
}
