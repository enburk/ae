#pragma once
#include <cmath>
#include <random>
#include <algorithm>
namespace aux
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
            value >= max ? max :
            std::clamp(T(value), min, max);
    }

    template <typename Int = int>
    Int random (Int l = min<Int>(), Int u = max<Int>())
    {
        thread_local std::random_device seed;
        thread_local std::mt19937 generator(seed());
        return std::uniform_int_distribution(l,u)(generator);
    }

    template<class T, int D> struct vector_data { std::array<T,D> data{}; void resize(int){} };
    template<class T> struct vector_data<T,1> { union { std::array<T,1> data{}; T x; }; void resize(int){} };
    template<class T> struct vector_data<T,2> { union { std::array<T,2> data{}; T x, y; }; void resize(int){} };
    template<class T> struct vector_data<T,3> { union { std::array<T,3> data{}; T x, y, z; }; void resize(int){} };
    template<class T> struct vector_data<T,4> { union { std::array<T,4> data{}; T x, y, z, w; }; void resize(int){} };
    template<class T> struct vector_data<T,0> { std::vector<T> data; void resize(int n){ data.resize(n); } };

    template<int D, class T> struct vector : vector_data<T,D>
    {
        using vector_data_= vector_data<T,D>;
        using vector_data_::data;

        vector() = default;
        template<int N, class U> vector (const U(&list)[N]) {
            static_assert(N == 0 || D == N, "wrong size of initializer list");
            vector_data_::resize(N); // do nothing if data is std::array
            int i = 0; for (auto value : list) data[i++] = clamp<T>(value);
        }
    };
}