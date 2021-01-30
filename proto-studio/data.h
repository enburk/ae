#pragma once
#include <array>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <variant>
#include <string>
namespace data
{
    template<class T> inline T min () { return std::numeric_limits<T>::lowest(); } // !!!
    template<class T> inline T max () { return std::numeric_limits<T>::max(); }
    template<class T> inline T min (T a, T b) { return (a < b ? a : b); }
    template<class T> inline T max (T a, T b) { return (a < b ? b : a); }
    template<class T> inline T min (T a, T b, T c) { return min(a, min(b, c)); }
    template<class T> inline T max (T a, T b, T c) { return max(a, max(b, c)); }
    template<class T> inline T min (T a, T b, T c, T d) { return min(a, min(b, min(c, d))); }
    template<class T> inline T max (T a, T b, T c, T d) { return max(a, max(b, max(c, d))); }

    template<class T, class U>
    T clamp (U value, T Min = min<T>(), T Max = max<T>()) {
        return
            value <= Min ? Min :
            value >= Max ? Max :
            std::clamp(T(value), Min, Max);
    }

    template <typename Int = int>
    Int random (Int l = min<Int>(), Int u = max<Int>())
    {
        thread_local std::random_device seed;
        thread_local std::mt19937 generator(seed());
        return std::uniform_int_distribution(l,u)(generator);
    }

///auto sum (auto... xs) { return (xs + ...); }

    struct polymorphic
    {
        virtual
       ~polymorphic             (                    ) = default;
        polymorphic             (                    ) = default;
        polymorphic             (polymorphic const& e) = default;
        polymorphic             (polymorphic     && e) = default;
        polymorphic& operator = (polymorphic const& e) = default;
        polymorphic& operator = (polymorphic     && e) = default;
    };

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

    struct exception : std::runtime_error {
        exception(std::string s) : std::runtime_error(
            std::move(s)) {}
    };

    template<class Value> struct expected : std::variant<Value, error>
    {
        using variant = std::variant<Value, error>;

        expected (Value v) : variant(std::move(v)) {}
        expected (error e) : variant(std::move(e)) {}

        bool ok  () const { return std::holds_alternative<Value>(*this); }
        bool bad () const { return std::holds_alternative<Error>(*this); }

        /***/ Value &  value () /***/ &  { if (ok()) return std::get<0>(/*******/(*this)); throw exception(std::get<1>(*this)); }
        /***/ Value && value () /***/ && { if (ok()) return std::get<0>(std::move(*this)); throw exception(std::get<1>(*this)); }
        const Value &  value () const &  { if (ok()) return std::get<0>(/*******/(*this)); throw exception(std::get<1>(*this)); }
        const Value && value () const && { if (ok()) return std::get<0>(std::move(*this)); throw exception(std::get<1>(*this)); }
        using Error =  error;
        /***/ Error &  error () /***/ &  { if (bad()) return std::get<1>(/*******/(*this)); throw exception("It was OK."); }
        /***/ Error && error () /***/ && { if (bad()) return std::get<1>(std::move(*this)); throw exception("It was OK."); }
        const Error &  error () const &  { if (bad()) return std::get<1>(/*******/(*this)); throw exception("It was OK."); }
        const Error && error () const && { if (bad()) return std::get<1>(std::move(*this)); throw exception("It was OK."); }
    };

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

    // https://en.cppreference.com/w/cpp/utility/variant/visit
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    namespace aux {
    template<class T, class... Types>
    constexpr bool got (const std::variant<Types...> & v) noexcept {
        return std::holds_alternative<T>(v);
    }}
}