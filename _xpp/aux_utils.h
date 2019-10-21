#pragma once
#include <vector>
#include <optional>
#include <algorithm>

// Sean Parent, CppCon 2019
// Russian coat check algorithm
template<class T> struct registry
{
    std::vector<
    std::pair<size_t,
    std::optional<T>>> pool;
    size_t size = 0, next_id = 0;

    auto append(T element) -> size_t {
        pool.emplace_back(next_id, std::move(element));
        ++size; return next_id++;
    }

    void erase(size_t id) noexcept {
        auto it = std::lower_bound(
            pool.begin(), pool.end(), id,
                [](const auto & a, const auto & b) { return a.first < b; });
        if (it == pool.end() || it->first != id || !it->second) return;
        it->second.reset(); --size;
        if (size < pool.size() / 2)
            pool.erase(std::remove_if(pool.begin(), pool.end(),
                [](const auto & e) { return !e.second; }), pool.end());
    }

    void clear() noexcept { pool.clear(); size = 0; }

    template<class F> void for_each(F f) {
        for (const auto & e : pool)
            if (e.second) f(*e.second);
    }
};

// C++ Core Guidelines C.21:
// If you define or delete any default operation,
// define or delete them all
struct polymorphic
{
    virtual
   ~polymorphic              (                      ) = default;
    polymorphic              (                      ) = default;
    polymorphic              (const polymorphic  & e) = default;
    polymorphic              (      polymorphic && e) = default;
    polymorphic & operator = (const polymorphic  & e) = default;
    polymorphic & operator = (      polymorphic && e) = default;
};

// Matteo Italia
// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
inline void hash_combine(std::size_t& seed) {}; template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) { std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
}
#define MAKE_HASHABLE(type, ...) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0; hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

