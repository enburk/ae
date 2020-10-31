#pragma once
#include "data_iterators.h"
#include <concepts>

template<class, class> inline constexpr bool same_as_v = false;
template<class x> inline constexpr bool same_as_v<x, x> = true;
template<class x, class y> concept same_as = same_as_v<x, y>;

auto sum (auto&&... ts) {
    return (ts + ...);
}

template<class X>
concept rrange = requires (X x){
    typename X::iterator;
    typename X::sentinel;
    { x.begin () }; // -> same_as<X::iterator>;
    { x.end   () }; // -> same_as<X::sentinel>;
};

template<class x>
bool ffff (rrange<x> r)
{
    auto i = r.begin();
    i++;
    return i == r.begin();
}

template<class x>
struct random_access_range : range<x>
{
    typedef range<x> base;
    typedef random_access_range same;
    typedef random_access_iterator<x> iterator;
    using base::begin;
    using base::end;

    auto from (iterator i) -> same;
    auto upto (iterator i) -> same;
    auto from (int n) { return from(begin()+n); }
    auto upto (int n) { return upto(begin()+n); }
    auto operator () (iterator i, iterator j) { return from(i).upto(j); }
    auto operator () (int b, int e) { return from(b).upto(e-b); }
    auto operator [] (int i) { return *(begin()+i); }
    auto operator () (int i) { return *(begin()+i); }
};

//template<class x>
//type contiguous.range(x) : random.access.range(x) =
//{
//    type iterator = contiguous.iterator; iterator begin;
//    type sentinel = contiguous.iterator; iterator end;
//
//    function size = end - begin;
//    function clip (iterator i) = min(max(begin,i),end);
//    function from (iterator i) = same(clip(i), end);
//    function upto (iterator i) = same(begin, clip(i));
//};
//
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
