#pragma once
#include <cmath>
#include <random>
#include <algorithm>
namespace pix
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
    T clamp (U value, T min = min<T>(), T max = max<T>()) {
        return
            value <= min ? min :
            value >= max ? max : std::clamp(T(value), min, max);
    }

    template <typename Int = int>
    Int random (Int l = min<Int>(), Int u = max<Int>())
    {
        thread_local std::random_device seed;
        thread_local std::mt19937 generator (seed ());
        return std::uniform_int_distribution (l, u) (generator);
    }
}