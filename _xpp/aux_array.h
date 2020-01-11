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
        void operator += (int n) { current += n; }
        void operator -= (int n) { current -= n; }
        bool operator == (iterator i) { return current == i.current; }
        bool operator != (iterator i) { return current != i.current; }
        std::pair<type, int> operator -> () { return **this; }
        std::pair<type, int> operator * () { return std::pair{*current, (int)(current-begin)}; }
        friend iterator operator + (iterator i, int n) { i.current += n; return i; }
        friend iterator operator - (iterator i, int n) { i.current -= n; return i; }
    };

    iterator begin () { return iterator{begin_, first}; }
    iterator end   () { return iterator{begin_, last }; }
    int      size  () { return (int)   (last -  first); }
};

template<class Iterator> struct range_with_ending
{
    using Range = range<Iterator>;
    using type = typename std::iterator_traits<Iterator>::value_type;
    Range range; type ending;

    struct iterator
    {
        int current, total;
        Iterator begin; type ending;
        void operator ++ () { ++current; }
        bool operator != (iterator i) { return current != i.current; }
        std::pair<type, int> operator * () { return current < total-1 ?
            std::pair{*(begin+current), current}:
            std::pair{  ending,         current}; }
    };

    iterator begin () { return iterator{0,              range.size()+1, range.first, ending}; }
    iterator end   () { return iterator{range.size()+1, range.size()+1, range.first, ending}; }
    int      size  () { return range.size() + 1; }
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

    int size () const { return (int) base::size (); }

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

    auto range ()             { return ::range<iterator>{begin(), begin(),   end(),     end()}; }
    auto range (int f)        { return ::range<iterator>{begin(), begin()+f, end(),     end()}; }
    auto range (int f, int n) { return ::range<iterator>{begin(), begin()+f, begin()+n, end()}; }

    friend std::ostream & operator << (std::ostream & out, const array & a) { for (const auto & e : a) out << e << std::endl; return out; }

    auto find         (const type & e) const { return std::find (begin(), end(), e); }
    bool found        (const type & e) const { return std::find (begin(), end(), e) != end(); }
    bool binary_found (const type & e) const { return std::binary_search (begin(), end(), e); }

    void try_emplace  (const type & e) { auto it = find(e); if (it == end()) base::push_back (e); }
    void try_erase    (const type & e) { auto it = find(e); if (it != end()) base::erase(it); }

    auto find_or_emplace (const type & e) {
        auto it = find(e); if (it != end()) return it;
        *this += e; it = end()-1; return it;
    }

    auto erase (int pos       ) { return base::erase(begin()+pos, begin()+pos+1); }
    auto erase (int pos, int n) { return base::erase(begin()+pos, begin()+pos+n); }
    auto erase (const_iterator f, const_iterator l) { return base::erase(f, l); }
    auto erase (const_iterator i                  ) { return base::erase(i); }

    template<class P> void erase_if (P predicate) { base::erase (std::remove_if(begin(), end(), predicate), end()); }
    
    template<class C> void sort (C compare = std::less<>{}) { std::sort(begin(), end(), compare); }

    template<class C> auto lower_bound (const type & e, C compare = std::less<>{}) { return std::lower_bound(begin(), end(), e, compare); }
    template<class C> auto upper_bound (const type & e, C compare = std::less<>{}) { return std::upper_bound(begin(), end(), e, compare); }

    template<class I> auto insert(int pos, I f, I l) { return base::insert(begin()+pos, f, l); }
    template<class I> auto insert(I   pos, I f, I l) { return base::insert(        pos, f, l); }
    template<class I> auto insert(I   pos, array  a) { return base::insert(        pos, a.begin(), a.end()); }

    template<class I> auto replace(int pos, int nn, I f, I l) { erase(pos, nn); return insert(pos, f, l); }
    template<class I> auto replace(I   pos, I   nn, I f, I l) { erase(pos, nn); return insert(pos, f, l); }
    template<class I> auto replace(I   pos, I   nn, array  a) { erase(pos, nn); return insert(pos, a); }

    auto interval (int pos, int n) { return array(begin()+pos, begin()+pos+n); }

    void truncate (int pos){ erase(pos, size()-pos); }
    void truncate (       ){ erase(size()-1); }

    array from (int pos) { return array(begin()+pos, end()); }
};


// template<class type> struct Array // rope-like B-tree of arrays
// {
//     std::map<int, array<type>> chunks; // actually: simplified red-black implementation
// };

template<class type> using Array = array<type>;

template<class type> using Range = range<typename Array<type>::iterator>;
