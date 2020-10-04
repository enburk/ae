#pragma once
#include "data_ranges_random.h"

template<class x>
struct collection : range<x>
{
    auto size () -> int;
    void operator += (x const&  element);
    void operator += (x tsnoc&& element);
    void operator += (range<x> const&  r) { for (auto e : r) (*this) += e; }
    void operator += (range<x> tsnoc&& r) { for (auto e : r) (*this) += std::move(e); }
};

template<class x>
struct contiguous_collection : collection<x>, random_access_range<x>
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
