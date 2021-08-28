#pragma once
#include <deque>
#include <vector>
#include "aux_v2_ranges.h"
#include "aux_v2_collections.h"
namespace aux
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
        void insert(iterator_ i, iterator_ b, iterator_ e) { container::insert(i, b, e); }

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
             container::resize(size()-1);
        }

        void deduplicate () {
            sort(std::less{});
            container::erase(std::unique(
                begin(), end()), end());
        }

        #include "aux_v2_algo_random.h"
        #include "aux_v2_algo_resizing.h"
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
