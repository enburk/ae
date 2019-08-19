#pragma once
#include <vector>
#include <iostream>
#include <algorithm>

template<class type> struct array : std::vector<type>
{
    using base = std::vector<type>;
    using base::vector;

    int size () const { return (int) base::size (); }

    void operator += (const type  & e) { base::push_back (e); }
    void operator += (const array & a) { base::insert (base::end(), a.cbegin(), a.cend()); }

    friend array operator + (const array & a, const type  & b) { array tt; tt += a; tt+= b; return tt; }
    friend array operator + (const type  & a, const array & b) { array tt; tt += a; tt+= b; return tt; }

    friend std::ostream & operator << (std::ostream & out, const array & a) { for (const auto & e : a) out << e << std::endl; return out; }

    bool found (const type & e) const { return std::find (base::begin(), base::end(), e) != base::end(); }
    bool binary_found (const type & e) const { return std::binary_search (base::begin(), base::end(), e); }
};

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
