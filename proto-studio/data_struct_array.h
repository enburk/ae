#pragma once
#include <deque>
#include <vector>
#include "data_ranges.h"
#include "data_collections.h"
#include "data_unittest.h"
namespace data
{
    template<class x> struct array : std::vector<x>
    {
        using container = std::vector<x>;

        using value_type = x;
        using iterator_= typename container::const_iterator;
        using iterator = typename container::iterator;
        using sentinel_= typename container::const_iterator;
        using sentinel = typename container::iterator;
        using range_type_= contiguous_collection_range_<array>;
        using range_type = contiguous_collection_range <array>;
        auto range (iterator_ i, iterator_ j) const { return range_type_{*this, i, j}; }
        auto range (iterator  i, iterator  j) /***/ { return range_type {*this, i, j}; }
        contiguous_range_impl(value_type);

        array () = default;
        array (array const&) = default;
        array (array     &&) = default;
        array (container const&c) : container(c) {}
        array (container     &&c) : container(std::move(c)) {}
        array (range_type r) : container(r.begin(), r.end()) {}
        array (std::initializer_list<x> list) : container(list) {}

        using container::begin;
        using container::end;
        using container::rbegin;
        using container::rend;

        auto from (int n) /***/ { return from(begin()+n); }
        auto upto (int n) /***/ { return upto(begin()+n); }
        auto from (int n) const { return from(begin()+n); }
        auto upto (int n) const { return upto(begin()+n); }

        array& operator = (array const& a) = default;
        array& operator = (array     && a) = default;

        void operator += (x e) { container::push_back(std::move(e)); }
        void operator += (array a) { container::insert(end(),
            std::make_move_iterator(a.begin()),
            std::make_move_iterator(a.end()));}
        template<class X>
        void operator += (X r)
            requires input_range<X> &&
            std::same_as<typename X::value_type, value_type> {
            for (const auto & e : r)
                (*this) += e;
        }
        template<class X>
        void operator += (X r)
            requires random_access_range<X> &&
            std::same_as<typename X::value_type, value_type> {
            reserve(size() + r.size());
            for (const auto & e : r)
                (*this) += e;
        }
        friend array operator + (array a, x b) { array r     = std::move(a); r += std::move(b); return r; }
        friend array operator + (x a, array b) { array r; r += std::move(a); r += std::move(b); return r; }

        void insert(iterator_ i, range_type r) { container::insert(i, r.begin(), r.end()); }
        void insert(iterator_ i, value_type e) { container::insert(i, std::move(e)); }

        void insert(int i, range_type r) { container::insert(begin()+i, r.begin(), r.end()); }
        void insert(int i, value_type e) { container::insert(begin()+i, std::move(e)); }

        void try_erase       (value_type e) { auto it = find(e); if (it != end()) container::erase(it); }
        void try_emplace     (value_type e) { auto it = find(e); if (it == end()) *this += e; }
        auto find_or_emplace (value_type e)
        {
            auto it = find(e); if (it != end()) return it;
            *this += e; it = end()-1; return it;
        }

        void triml (value_type const& e){
             array ee; ee += e;
             upto(first(one_not_of(ee))
                .begin()).erase();
        }
        void trimr (value_type const& e){
             array ee; ee += e;
             from(last(one_not_of(ee))
                .end()).erase();
        }
        void strip (value_type const& e){
             trimr(e); triml(e);
        }
        void truncate () { if (size() > 0)
             resize(size()-1);
        }

        #include "data_algo_random.h"
        #include "data_algo_resizing.h"
    };

    template<class type> struct deque : public std::deque<type>
    {
        using base = std::deque<type>;

        int size () const { return (int)(base::size()); }

        deque (            ) = default;
        deque (deque const&) = default;
        deque (deque     &&) = default;
        deque (base const&c) : base(c) {}
        deque (base     &&c) : base(std::move(c)) {}

        explicit deque (array<type> const& a) : base(a.begin(), a.end()) {}
        explicit deque (array<type>     && a) : base(
            std::make_move_iterator(a.begin()),
            std::make_move_iterator(a.end())) {}

        auto& operator =  (deque const& a) { base::operator = (a); return *this; }
        auto& operator =  (deque     && a) { base::operator = (std::move(a)); return *this; }

        void  operator += (type  const& e) { base::push_back(e); }
        void  operator += (type      && e) { base::push_back(std::move(e)); }

        void  operator += (deque const& a) { base::insert(base::end(), a.begin(), a.end()); }
        void  operator += (deque     && a) { base::insert(base::end(),
            std::make_move_iterator(a.begin()),
            std::make_move_iterator(a.end())); }
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
        test("array.compare");
        {
            array<int> a = {1, 2, 3};
            array<int> b = {2, 3, 4};
            oops( out(a == a) ) { "1" };
            oops( out(a != b) ) { "1" };
            oops( out(a <= b) ) { "1" };
            oops( out(a != b.upto(3)) ) { "1" };
            oops( out(a <= b.upto(3)) ) { "1" };
            oops( out(a.from(0) != b) ) { "1" };
            oops( out(a.from(0) <= b) ) { "1" };
            oops( out(a.from(0) != b.upto(3)) ) { "1" };
            oops( out(a.from(0) <= b.upto(3)) ) { "1" };
            oops( out(a.from(1) == b.upto(2)) ) { "1" };
            oops( out(a.from(1).starts_with(b)) ) { "0" };
            oops( out(a.from(1).starts_with(b.upto(2))) ) { "1" };
            oops( out(a.from(0).ends_with(b.upto(3))) ) { "0" };
            oops( out(a.from(0).ends_with(b.upto(2))) ) { "1" };
        }
        test("array.erase");
        {
            array<int> a = {1, 2, 3};
            oops( a.from(0).upto(0).erase(); out(a) ) { "1, 2, 3" };
            oops( a.from(0).upto(1).erase(); out(a) ) { "2, 3" };
            oops( a.from(0).upto(2).erase(); out(a) ) { "" };
            oops( a.from(0).upto(2).erase(); out(a) ) { "" };
        }
        test("array.strip");
        {
            array<int> a = {1, 2, 3, 3, 2, 1};
            oops( a.strip(2); out(a) ) { "1, 2, 3, 3, 2, 1" };
            oops( a.strip(1); out(a) ) { "2, 3, 3, 2" };
            oops( a.triml(2); out(a) ) { "3, 3, 2" };
            oops( a.trimr(2); out(a) ) { "3, 3" };
            oops( a.trimr(3); out(a) ) { "" };
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
            a = {1, 2, 3};
            oops( a.upto(0) = b.from(3); out(a) ) { "1, 2, 3" };
            oops( a.upto(1) = b.upto(1); out(a) ) { "4, 2, 3" };
            oops( a.upto(1) = b.from(1); out(a) ) { "5, 6, 2, 3" };
            oops( a.upto(2) = b.from(2); out(a) ) { "6, 2, 3" };
            oops( a.upto(3) = b.upto(3); out(a) ) { "4, 5, 6" };
            oops( a.upto(0) = b.from(0); out(a) ) { "4, 5, 6, 4, 5, 6" };
        }
    }
    catch(assertion_failed){}
}
