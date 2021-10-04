#pragma once
#ifdef  __clang__
#define   aux_v2
#include "aux_v2_array.h"
#include "aux_v2_string.h"
#else
#define   aux_v1
#include "aux_v1_array.h"
#include "aux_v1_string.h"
#endif
#include "aux_coro.h"
#include "aux_registry.h"
#include "aux_unicode.h"

using aux::max;
using aux::min;
using aux::clamp;
using aux::polymorphic;
using aux::overloaded;
using aux::expected;
using aux::nothing;
using aux::input_range;
using aux::random_access_range;
using aux::deque;
using aux::array;
using aux::str;
using aux::one_of;
using aux::one_not_of;
using aux::enumerate;
using aux::generator;
using aux::task;

