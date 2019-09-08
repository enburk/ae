#pragma once
#include <map>
#include <vector>
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
        Iterator current;
        Iterator begin, end; char ending;
        void operator ++ () { ++current; }
        bool operator != (iterator i) { return current != i.current; }
        std::pair<type, long> operator * () { return current < end ?
            std::pair{*current, (long)(current-begin)} :
            std::pair{ ending,  (long)(end-begin)}; }
    };

    iterator begin () { return iterator{range.first,  range.first, range.last, ending}; }
    iterator end   () { return iterator{range.last+1, range.first, range.last, ending}; }
    long     size  () { return range.size() + 1; }
};

template<class I> range_with_ending<I> operator + (range<I> r, typename range<I>::type e) { return range_with_ending<I>{r, e}; }

template<class type> struct array : std::vector<type>
{
    using base = std::vector<type>;
    using base::vector;
    using base::begin;
    using base::end;

    long size () const { return (long) base::size (); }

    void operator += (const type  & e) { base::push_back (e); }
    void operator += (const array & a) { base::insert (end(), a.cbegin(), a.cend()); }

    friend array operator + (const array & a, const type  & b) { array tt; tt += a; tt+= b; return tt; }
    friend array operator + (const type  & a, const array & b) { array tt; tt += a; tt+= b; return tt; }

    auto range ()               { return ::range<base::iterator>{begin(), begin(),   end(),     end()}; }
    auto range (long f)         { return ::range<base::iterator>{begin(), begin()+f, end(),     end()}; }
    auto range (long f, long n) { return ::range<base::iterator>{begin(), begin()+f, begin()+n, end()}; }

    friend std::ostream & operator << (std::ostream & out, const array & a) { for (const auto & e : a) out << e << std::endl; return out; }

    auto find         (const type & e) const { return std::find (begin(), end(), e); }
    bool found        (const type & e) const { return std::find (begin(), end(), e) != end(); }
    bool binary_found (const type & e) const { return std::binary_search (begin(), end(), e); }
};


// template<class type> struct Array // rope-like B-tree of arrays
// {
//     std::map<int, array<type>> chunks; // actually: simplified red-black implementation
// };

template<class type> using Array = array<type>;

template<class type> using Range = range<typename Array<type>::iterator>;

// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
inline void hash_combine(std::size_t& seed) {}; template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
}
#define MAKE_HASHABLE(type, ...) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0;\
                hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

template<class T, class U> T clamp
(
    U value,
    T min = std::numeric_limits<T>::min(),
    T max = std::numeric_limits<T>::max())
{
    return
        value <= U(min) ? min :
        value >= U(max) ? max : std::clamp(T(value), min, max);
}
