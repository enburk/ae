#pragma once
#include <map>
#include <vector>
#include <optional>
#include <iostream>
#include <algorithm>

template<class Iterator> struct range
{
    Iterator begin_, first, last, end_;

    using type = typename std::iterator_traits<Iterator>::value_type;

    struct iterator
    {
        Iterator begin;
        Iterator current;
        void operator ++ () { ++current; }
        void operator -- () { --current; }
        void operator += (long n) { current += n; }
        void operator -= (long n) { current -= n; }
        bool operator == (iterator i) { return current == i.current; }
        bool operator != (iterator i) { return current != i.current; }
        std::pair<type, long> operator -> () { return **this; }
        std::pair<type, long> operator * () { return std::pair{*current, (long)(current-begin)}; }
        friend iterator operator + (iterator i, long n) { i.current += n; return i; }
        friend iterator operator - (iterator i, long n) { i.current -= n; return i; }
    };

    iterator begin () { return iterator{begin_, first}; }
    iterator end   () { return iterator{begin_, last }; }
    long     size  () { return (long)  (last -  first); }
};

template<class Iterator> struct range_with_ending
{
    using Range = range<Iterator>;
    using type = typename std::iterator_traits<Iterator>::value_type;
    Range range; type ending;

    struct iterator
    {
        long current, total;
        Iterator begin; type ending;
        void operator ++ () { ++current; }
        bool operator != (iterator i) { return current != i.current; }
        std::pair<type, long> operator * () { return current < total-1 ?
            std::pair{*(begin+current), current}:
            std::pair{  ending,         current}; }
    };

    iterator begin () { return iterator{0,              range.size()+1, range.first, ending}; }
    iterator end   () { return iterator{range.size()+1, range.size()+1, range.first, ending}; }
    long     size  () { return range.size() + 1; }
};

template<class I> range_with_ending<I> operator + (range<I> r, typename range<I>::type e) { return range_with_ending<I>{r, e}; }

template<class type> struct array : std::vector<type>
{
    using base = std::vector<type>;
    using typename base::const_iterator;
    using typename base::iterator;
    using base::vector;
    using base::begin;
    using base::end;

    array (                ) : base ( ) {}
    array (const base  &  a) : base (a) {}
    array (      base  && a) : base (std::move(a)) {}
    array (const array &  a) = default;
    array (      array && a) = default;
    array (const_iterator f, const_iterator l) : base (f, l) {}

    long size () const { return (long) base::size (); }

    array & operator = (const array &  s) { base::operator = (s); return *this; }
    array & operator = (      array && s) { base::operator = (std::move(s)); return *this; }

    void operator += (const type   & e) { base::emplace_back (e); }
    void operator += (      type  && e) { base::emplace_back (std::move(e)); }

    void operator += (const array &  a) { base::insert (end(), a.cbegin(), a.cend()); }
    void operator += (      array && a) { base::insert (end(),
        std::make_move_iterator(a.begin()),
        std::make_move_iterator(a.end())); }

    friend array operator + (const array & a, const type  & b) { array tt; tt += a; tt+= b; return tt; }
    friend array operator + (const type  & a, const array & b) { array tt; tt += a; tt+= b; return tt; }

    auto range ()               { return ::range<iterator>{begin(), begin(),   end(),     end()}; }
    auto range (long f)         { return ::range<iterator>{begin(), begin()+f, end(),     end()}; }
    auto range (long f, long n) { return ::range<iterator>{begin(), begin()+f, begin()+n, end()}; }

    friend std::ostream & operator << (std::ostream & out, const array & a) { for (const auto & e : a) out << e << std::endl; return out; }

    auto find         (const type & e) const { return std::find (begin(), end(), e); }
    bool found        (const type & e) const { return std::find (begin(), end(), e) != end(); }
    bool binary_found (const type & e) const { return std::binary_search (begin(), end(), e); }

    template<class P> void erase_if (P predicate) { base::erase (std::remove_if(begin(), end(), predicate), end()); }
    
    void try_emplace  (const type & e) { auto it = find(e); if (it == end()) base::push_back (e); }
    void try_erase    (const type & e) { auto it = find(e); if (it != end()) base::erase(it); }
};


// template<class type> struct Array // rope-like B-tree of arrays
// {
//     std::map<int, array<type>> chunks; // actually: simplified red-black implementation
// };

template<class type> using Array = array<type>;

template<class type> using Range = range<typename Array<type>::iterator>;
