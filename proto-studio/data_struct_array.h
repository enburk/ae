#pragma once
#include <vector>
#include "data_ranges.h"
#include "data_collections.h"
#include "data_unittest.h"
namespace data
{
    template<class x> struct array
    {
        using
        container = std::vector<x>;
        container data;

        using value_type = x;
        using iterator_= typename container::const_iterator;
        using iterator = typename container::iterator;
        using sentinel_= typename container::const_iterator;
        using sentinel = typename container::iterator;
        using range_type_= contiguous_collection_range_<array>;
        using range_type = contiguous_collection_range <array>;
        auto range (iterator_ i, iterator_ j) const { return range_type_{*this, i, j}; }
        auto range (iterator  i, iterator  j) /***/ { return range_type {*this, i, j}; }
        auto begin () /***/ { return data.begin(); }
        auto end   () /***/ { return data.end  (); }
        auto begin () const { return data.begin(); }
        auto end   () const { return data.end  (); }
        contiguous_range_impl(value_type);

        array () = default;
        array (std::initializer_list<x> list) : data(list) {}

        void resize  (int n) { data.resize (n); }
        void reserve (int n) { data.reserve(n); }

        auto from (int n) { return from(begin()+n); }
        auto upto (int n) { return upto(begin()+n); }
        auto from (int n) const { return from(begin()+n); }
        auto upto (int n) const { return upto(begin()+n); }

        void operator += (x e) { data.push_back(std::move(e)); }
        void operator += (array a) { data.insert(data.end(),
            std::make_move_iterator(a.data.begin()),
            std::make_move_iterator(a.data.end()));}
        void operator += (input_range auto r) {
            for (const auto & e : r)
                (*this) += e;
        }
        friend array operator + (array a, x b) { array r     = std::move(a); r += std::move(b); return r; }
        friend array operator + (x a, array b) { array r; r += std::move(a); r += std::move(b); return r; }

        void erase (iterator f, iterator l)
        {
            data.erase(
            data.begin() + (f - begin()),
            data.begin() + (l - begin()));
        }

        void insert (iterator i, range_type r)
        {
            data.insert(
            data.begin() + (i - begin()),
                r.begin(),
                r.end());
        }
        void insert (iterator i, value_type e)
        {
            data.insert(
            data.begin() + (i - begin()),
                std::move(e));
        }

        #include "data_algo_random.h"
        #include "data_algo_resizing.h"
    };
}

#include "data_unittest.h"
namespace data::unittest
{
    void test_array () try
    {
        test("array.append");
        {
            array<int> a, b, c;
            oops( a += 1;            out(a) ) { "1" };
            oops( b += a;            out(b) ) { "1" };
            oops( b += b;            out(b) ) { "1, 1" };
            oops( c += std::move(b); out(c) ) { "1, 1" };
            oops( b += 2;            out(b) ) { "2" };
        }
        test("array.ranges");
        {
            array<int> a = {1, 2, 3};
            oops( out(a.from(0)) ) { "1, 2, 3" };
            oops( out(a.from(1)) ) { "2, 3" };
            oops( out(a.from(2)) ) { "3" };
            oops( out(a.from(3)) ) { "" };
            oops( out(a.from(0).upto(0)) ) { "" };
            oops( out(a.from(0).upto(1)) ) { "1" };
            oops( out(a.from(0).upto(2)) ) { "1, 2" };
            oops( out(a.from(0).upto(3)) ) { "1, 2, 3" };
            oops( out(a.from(1).upto(1)) ) { "" };
            oops( out(a.from(1).upto(2)) ) { "2" };
            oops( out(a.from(1).upto(3)) ) { "2, 3" };
            oops( out(a.from(2).upto(2)) ) { "" };
            oops( out(a.from(2).upto(3)) ) { "3" };
            oops( out(a.from(3).upto(3)) ) { "" };
            oops( out(a(0, 0)) ) { "" };
            oops( out(a(0, 1)) ) { "1" };
            oops( out(a(0, 2)) ) { "1, 2" };
            oops( out(a(0, 3)) ) { "1, 2, 3" };
            oops( out(a(1, 1)) ) { "" };
            oops( out(a(1, 2)) ) { "2" };
            oops( out(a(1, 3)) ) { "2, 3" };
            oops( out(a(2, 2)) ) { "" };
            oops( out(a(2, 3)) ) { "3" };
            oops( out(a(3, 3)) ) { "" };
        }
        test("array.erase");
        {
            array<int> a = {1, 2, 3};
            oops( a.from(0).upto(0).erase(); out(a) ) { "1, 2, 3" };
            oops( a.from(0).upto(1).erase(); out(a) ) { "2, 3" };
            oops( a.from(0).upto(2).erase(); out(a) ) { "" };
            oops( a.from(0).upto(2).erase(); out(a) ) { "" };
        }
        test("array.insert");
        {
            array<int> a = {};
            oops( a.insert(a.begin()+0, 2); out(a) ) { "2" };
            oops( a.insert(a.begin()+1, 3); out(a) ) { "2, 3" };
            oops( a.insert(a.begin()+0, 1); out(a) ) { "1, 2, 3" };
        }
        test("array.replace");
        {
            array<int> a = {1, 2, 3};
            array<int> b = {4, 5, 6};
            oops( a.upto(0).replace_by(b.from(3)); out(a) ) { "1, 2, 3" };
            oops( a.upto(1).replace_by(b.upto(1)); out(a) ) { "4, 2, 3" };
            oops( a.upto(1).replace_by(b.from(1)); out(a) ) { "5, 6, 2, 3" };
            oops( a.upto(2).replace_by(b.from(2)); out(a) ) { "6, 2, 3" };
            oops( a.upto(3).replace_by(b.upto(3)); out(a) ) { "4, 5, 6" };
            oops( a.upto(0).replace_by(b.from(0)); out(a) ) { "4, 5, 6, 4, 5, 6" };
        }
    }
    catch(assertion_failed){}
}
