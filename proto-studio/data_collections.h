#pragma once
#include "data_ranges.h"

template<class x>
struct collection : range<x>
{
    typedef range<x> range;
    typedef collection same;

    auto size () -> int;
//    #define collection_impl \
//    friend same operator + (same  l, x     r) { same c; c += l; c += r; return c; } \
//    friend same operator + (same  l, range r) { same c; c += l; c += r; return c; } \
//    friend same operator + (same  l, same  r) { same c; c += l; c += r; return c; } \
//    friend same operator + (range l, same  r) { same c; c += l; c += r; return c; } \
//    friend same operator + (x     l, same  r) { same c; c += l; c += r; return c; } \
//    void operator += (same && c) { for (auto && e : c) (*this) += e; } \
//    void operator += (range   r) { for (auto & e : r) (*this) += e; }
//    void operator += (x const&  element);
//    void operator += (x tsnoc&& element);
};



template<class x>
struct contiguous_collection :  random_access_range<x> // collection<x>,
{
    typedef contiguous_iterator<x> iterator;
    typedef contiguous_iterator<x> sentinel;

    auto size () const -> int;
    void size (int);

    // function first = this[0];
    // function last  = this[size-1];
    // 
    // operator ~= (c: collection(x)) =
    // {
    //     i := end;
    //     size += c.size;
    //     for each e in c do {
    //         i::value ← e;
    //         i++;
    //     }
    // }
    // void operator += (x const&  element);
    // void operator += (x tsnoc&& element);
    // 
    // operator += (e: x) =
    // {
    //     size++; last = e;
    // }
    // 
    // ;; mutation erase =
};
