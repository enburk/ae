#pragma once

///auto sum (auto... xs) { return (xs + ...); }

//template<class T> inline T min () { return std::numeric_limits<T>::lowest(); } // !!!
//template<class T> inline T max () { return std::numeric_limits<T>::max(); }
//template<class T> inline T min (T a, T b) { return (a < b ? a : b); }
//template<class T> inline T max (T a, T b) { return (a < b ? b : a); }
//inline auto min (auto a, auto b, auto... xs) { return min(min(a, b), xs...); }
//inline auto max (auto a, auto b, auto... xs) { return max(max(a, b), xs...); }

#include "aux_abc.h"
using namespace aux;
