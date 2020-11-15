#pragma once
#include <vector>
#include "data_ranges.h"
//#include "data_collections.h"


namespace tst
{
template<class x>//, class container = std::vector<x>>
struct array //: contiguous_collection<x>
{
    typedef std::vector<x> container;

    container data;

    typedef array same;
    //typedef range<x> range;
    //typedef contiguous_collection<x> base;
    //using typename base::iterator;
    //using typename base::sentinel;
    typedef contiguous_iterator<x> iterator;
    typedef contiguous_iterator<x> sentinel;


    auto size () const { return (int) data.size(); }
    void size (int n) { data.resize(n); }

    auto begin () { return iterator{.pointer = data.data() }; }
    auto end   () { return sentinel{ begin() + size() }; }

    auto clip (iterator i) { return min(max(begin(),i),end()); }
    auto from (iterator i) { return contiguous_range<x>{clip(i), end()}; }
    auto upto (iterator i) { return contiguous_range<x>{begin(), clip(i)}; }

    void operator += (x const&  element) { data.push_back(element); }
    void operator += (x /***/&& element) { data.push_back(std::move(element)); }
    void operator += (range auto r) { for (const auto & e : r) (*this) += e; }

//     //collection_impl;
//     friend same operator + (same  l, x     r) { same c; c += l; c += r; return c; }
//     friend same operator + (same  l, range r) { same c; c += l; c += r; return c; }
//     friend same operator + (same  l, same  r) {
//         same c;
//         c += l;
//         c += r;
//         return c; }
//     friend same operator + (range l, same  r) { same c; c += l; c += r; return c; }
//     friend same operator + (x     l, same  r) { same c; c += l; c += r; return c; }
// //    void operator += (same && c) { for (auto && e : c) (*this) += e; }

    random_access_range_impl
    #include "data_ranges_forward.h"
    #include "data_ranges_random.h"
};
}
