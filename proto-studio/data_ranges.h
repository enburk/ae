#pragma once
#include "data_aux.h"
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

template<class x>
struct contiguous_range // random_access_range
{
    using same = contiguous_range;
    using iterator = contiguous_iterator<x>;
    using sentinel = contiguous_iterator<x>;
    iterator begin_; auto begin () { return begin_; }
    sentinel end_;   auto end   () { return end_;   }

    auto size () { return end() - begin(); }
    auto clip (iterator i) { return min(max(begin(),i),end()); }
    auto from (iterator i) { return same{clip(i), end()}; }
    auto upto (iterator i) { return same{begin(), clip(i)}; }
    random_access_range_impl
    #include "data_ranges_forward.h"
    #include "data_ranges_random.h"
};

struct integer_range // random_access_range // immutable
{
    using same = integer_range;
    using iterator = integer_iterator;
    using sentinel = integer_iterator;
    iterator begin_; auto begin () { return begin_; }
    sentinel end_;   auto end   () { return end_;   }

    auto size () { return end() - begin(); }
    auto clip (iterator i) { return min(max(begin(),i),end()); }
    auto from (iterator i) { return same{clip(i), end()}; }
    auto upto (iterator i) { return same{begin(), clip(i)}; }
    random_access_range_impl
};
