#pragma once
#include "data_iterators.h"
#include <concepts>

template<class X>
concept range = requires (X x){
    typename X::iterator;
    typename X::sentinel;
    { x.begin () };
    { x.end   () };
};

template<class X>
concept random_access_range = range<X> && requires (X x, int i, int j)
{
    { x.from (i) };
    { x.upto (i) };
    { x.from (X::iterator) };
    { x.upto (X::iterator) };
    { x (X::iterator, X::iterator) };
    { x (i, j) };
    { x [i] };
    { x (i) };
    #define random_access_range_impl \
    auto from (int n) { return from(begin()+n); } \
    auto upto (int n) { return upto(begin()+n); } \
    auto operator () (iterator i, iterator j) { return from(i).upto(j); } \
    auto operator () (int b, int e) { return from(b).upto(e-b); } \
    auto operator [] (int i) { return *(begin()+i); } \
    auto operator () (int i) { return *(begin()+i); }
};

template<class x> // requires random_access_range<contiguous_range<x>>
struct contiguous_range
{
    using same = contiguous_range;
    using iterator = contiguous_iterator<x>; iterator begin;
    using sentinel = contiguous_iterator<x>; iterator end;

    auto size () { return end() - begin(); }
    auto clip (iterator i) { return min(max(begin(),i),end()); }
    auto from (iterator i) { return same{clip(i), end()}; }
    auto upto (iterator i) { return same{begin(), clip(i)}; }
    random_access_range_impl
    #include "data_ranges_forward.h"
    #include "data_ranges_random.h"
};

//type integer.range : random.access.range(x) = ;; immutable
//{
//    type iterator = integer.iterator; iterator begin;
//    type sentinel = integer.iterator; iterator end;
//
//    function size = end - begin;
//    function clip (iterator i) = min(max(begin,i),end);
//    function from (iterator i) = same(clip(i), end);
//    function upto (iterator i) = same(begin, clip(i));
//};
