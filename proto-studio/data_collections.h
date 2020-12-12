#pragma once
#include "data_ranges.h"

template<class X> struct
contiguous_collection_range_
{
    const X& host;
    using iterator = typename X::iterator_;
    using sentinel = typename X::sentinel_;
    using iterator_ = typename X::iterator_;
    using sentinel_ = typename X::sentinel_;
    using value_type = typename X::value_type;
    using range_type = contiguous_collection_range_;
    auto range (iterator i, iterator j) { return range_type{host, i, j}; }

    iterator  begin_;
    sentinel  end_;
    iterator  begin () { return begin_; }
    sentinel  end   () { return end_;   }
    iterator_ begin () const { return begin_; }
    sentinel_ end   () const { return end_;   }
    contiguous_range_impl(value_type);

    auto upto (int n) { return range(begin(), host.clip(host.begin() + n)); }
    auto span (int n) { return range(begin(), host.clip(     begin() + n)); }

    auto offset () { return begin() - host.begin(); }

    #include "data_algo_random.h"
};

template<class X> struct
contiguous_collection_range
{
    X& host;
    using iterator = typename X::iterator;
    using sentinel = typename X::sentinel;
    using iterator_ = typename X::iterator_;
    using sentinel_ = typename X::sentinel_;
    using value_type = typename X::value_type;
    using range_type = contiguous_collection_range;
    auto range (iterator  i, iterator  j) { return range_type {host, i, j}; }

    iterator  begin_;
    sentinel  end_;
    iterator  begin () { return begin_; }
    sentinel  end   () { return end_;   }
    iterator_ begin () const { return begin_; }
    sentinel_ end   () const { return end_;   }
    contiguous_range_impl(value_type);

    auto upto (int n) { return range(begin(), host.clip(host.begin() + n)); }
    auto span (int n) { return range(begin(), host.clip(     begin() + n)); }

    auto offset () { return begin() - host.begin(); }

    void insert (iterator i, range_type r) { host.insert(i, r); }
    void insert (iterator i, value_type e) { host.insert(i, std::move(e)); }
    void erase  () { host.erase(begin(), end()); end_ = begin_; }

    #include "data_algo_random.h"
    #include "data_algo_resizing.h"
};



// template<class x>
// struct collection : range<x>
// {
//     typedef range<x> range;
//     typedef collection same;
// 
//     auto size () -> int;
// //    #define collection_impl \
// //    friend same operator + (same  l, x     r) { same c; c += l; c += r; return c; } \
// //    friend same operator + (same  l, range r) { same c; c += l; c += r; return c; } \
// //    friend same operator + (same  l, same  r) { same c; c += l; c += r; return c; } \
// //    friend same operator + (range l, same  r) { same c; c += l; c += r; return c; } \
// //    friend same operator + (x     l, same  r) { same c; c += l; c += r; return c; } \
// //    void operator += (same && c) { for (auto && e : c) (*this) += e; } \
// //    void operator += (range   r) { for (auto & e : r) (*this) += e; }
// //    void operator += (x const&  element);
// //    void operator += (x tsnoc&& element);
// };
// 

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
// 
// 
// template<class x>
// struct contiguous_collection :  random_access_range<x> // collection<x>,
// {
//     typedef contiguous_iterator<x> iterator;
//     typedef contiguous_iterator<x> sentinel;
// 
//     auto size () const -> int;
//     void size (int);
// 
//     // function first = this[0];
//     // function last  = this[size-1];
//     // 
//     // operator ~= (c: collection(x)) =
//     // {
//     //     i := end;
//     //     size += c.size;
//     //     for each e in c do {
//     //         i::value ← e;
//     //         i++;
//     //     }
//     // }
//     // void operator += (x const&  element);
//     // void operator += (x tsnoc&& element);
//     // 
//     // operator += (e: x) =
//     // {
//     //     size++; last = e;
//     // }
//     // 
//     // ;; mutation erase =
// };
