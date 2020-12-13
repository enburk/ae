#pragma once
#include "data.h"
#include "data_iterators.h"
#include <concepts>

template<class X> concept
input_range = requires (X x)
{
    typename X::iterator; { x.begin () };
    typename X::sentinel; { x.end   () };
};

template<class X> concept
forward_range = input_range<X>;

template<class X> concept
bidirectional_range = forward_range<X>;

template<class X> concept
random_access_range = bidirectional_range<X> && requires (X x)
{
    typename X::value_type;
    typename X::range_type;
    //{ x.from (X::iterator) };
    //{ x.upto (X::iterator) };
    #define random_access_range_impl(x) \
    auto operator () (iterator i, iterator j) const { return from(i).upto(j); } \
    auto operator () (iterator i, iterator j) /***/ { return from(i).upto(j); } \
    auto operator () (int b, int e) const { return from(begin()+b).upto(begin()+e); } \
    auto operator () (int b, int e) /***/ { return from(begin()+b).upto(begin()+e); } \
    auto operator [] (int i) /***/ -> x /***/& { return *(begin()+i); } \
    auto operator [] (int i) const -> x const& { return *(begin()+i); } \
    auto operator () (int i) { return *(begin()+i); }
};

template<class X> concept
contiguous_range = random_access_range<X> && requires (X x)
{
    { x.range (X::iterator, X::iterator) };
    #define contiguous_range_impl(x) \
    auto size () const { return (int)(end() - begin()); } \
    auto clip (iterator_ i) const { return std::min(std::max(begin(),i), end()); } \
    auto clip (iterator  i) /***/ { return std::min(std::max(begin(),i), end()); } \
    auto from (iterator_ i) const { return range(clip(i), end  ()); } \
    auto from (iterator  i) /***/ { return range(clip(i), end  ()); } \
    auto upto (iterator_ i) const { return range(begin(), clip(i)); } \
    auto upto (iterator  i) /***/ { return range(begin(), clip(i)); } \
    random_access_range_impl(x);
};

bool operator == (
    input_range auto const& l,
    input_range auto const& r) {
    return std::equal(
        l.begin(), l.end(),
        r.begin(), r.end());
}
auto operator <=> (
    input_range auto const& l,
    input_range auto const& r) {
    return std::lexicographical_compare_three_way(
        l.begin(), l.end(),
        r.begin(), r.end());
}


//template<class x> struct
//memory_range // contiguous_range
//{
//    using iterator = contiguous_iterator<x>;
//    using sentinel = contiguous_iterator<x>;
//    using range_type = memory_range;
//    using value_type = x;
//    auto range (iterator i, iterator j) const { return memory_range{i, j}; }
//    auto range (iterator i, iterator j) /***/ { return memory_range{i, j}; }
//    iterator begin_; sentinel end_;
//    auto begin () { return begin_; }
//    auto end   () { return end_;   }
//    auto begin () const { return begin_; }
//    auto end   () const { return end_;   }
//    contiguous_range_impl(x);
//    #include "data_algo_random.h"
//};
//
//struct
//integer_range // contiguous_range // immutable
//{
//    using iterator = integer_iterator;
//    using sentinel = integer_iterator;
//    using range_type = integer_range;
//    using value_type = int;
//    auto range (iterator i, iterator j) const { return integer_range{i, j}; }
//    iterator begin_; sentinel end_;
//    auto begin () const { return begin_; }
//    auto end   () const { return end_;   }
//    contiguous_range_impl(int);
//};
