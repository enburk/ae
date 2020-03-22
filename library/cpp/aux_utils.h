#pragma once
#include <deque>
#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <algorithm>
namespace aux
{
    struct nothing {};

    struct error : std::string {
        using string = std::string;
        using string::string;
        error(string s) : string(std::move(s)) {}
        auto operator = (string s) {
            string::operator = (std::move(s));
            return *this;
        }
    };

    template<class Value> struct expected : std::variant<Value, error>
    {
        using variant = std::variant<Value, error>;

        expected (Value v) : variant(std::move(v)) {}
        expected (error e) : variant(std::move(e)) {}

        bool ok  () const { return std::holds_alternative<Value>(*this); }
        bool bad () const { return std::holds_alternative<error>(*this); }

        /***/ Value &  value () /***/ &  { if (ok()) return std::get<0>(/*******/(*this)); throw std::runtime_error(std::get<1>(*this)); }
        /***/ Value && value () /***/ && { if (ok()) return std::get<0>(std::move(*this)); throw std::runtime_error(std::get<1>(*this)); }
        const Value &  value () const &  { if (ok()) return std::get<0>(/*******/(*this)); throw std::runtime_error(std::get<1>(*this)); }
        const Value && value () const && { if (ok()) return std::get<0>(std::move(*this)); throw std::runtime_error(std::get<1>(*this)); }
        using Error =  error;
        /***/ Error &  error () /***/ &  { if (ok()) return std::get<1>(/*******/(*this)); throw std::runtime_error("It was OK."); }
        /***/ Error && error () /***/ && { if (ok()) return std::get<1>(std::move(*this)); throw std::runtime_error("It was OK."); }
        const Error &  error () const &  { if (ok()) return std::get<1>(/*******/(*this)); throw std::runtime_error("It was OK."); }
        const Error && error () const && { if (ok()) return std::get<1>(std::move(*this)); throw std::runtime_error("It was OK."); }
    };

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

    template<class type> struct deque : public std::deque<type>
    {
        using base = std::deque<type>;

        int size () const { return (int) base::size(); }

        void operator += (const type  & e) { base::push_back(e); }
        void operator += (const deque & a) { base::insert(base::end(), a.begin(), a.end()); }
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
                std::size_t ret = 0; aux::hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

    namespace unicode
    {
        array<str> glyphs (str s)
        {
            array<str> glyphs;
            glyphs.reserve(s.size());

            auto check = [&s](auto i){ if (i == s.end())
            throw std::runtime_error("unicode: broken UTF-8"); };

            for (auto i = s.begin(); i != s.end(); )
            {
                char c = *i++; str g = c;
                uint8_t u = static_cast<uint8_t>(c);
                if ((u & 0b11000000) == 0b11000000) { check(i); g += *i++; // 110xxxxx 10xxxxxx
                if ((u & 0b11100000) == 0b11100000) { check(i); g += *i++; // 1110xxxx 10xxxxxx 10xxxxxx
                if ((u & 0b11110000) == 0b11110000) { check(i); g += *i++; // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                }}}
                glyphs += g;
            }
            return glyphs;
        }

        int length (str s)
        {
            int n = 0;
            for (auto i = s.begin(); i != s.end(); )
            {
                char c = *i++; n++;
                uint8_t u = static_cast<uint8_t>(c);
                if ((u & 0b11000000) == 0b11000000) { if (i == s.end()) break; n++; i++;
                if ((u & 0b11100000) == 0b11100000) { if (i == s.end()) break; n++; i++;
                if ((u & 0b11110000) == 0b11110000) { if (i == s.end()) break; n++; i++;
                }}}
            }
            return n;
        }
    }
}

