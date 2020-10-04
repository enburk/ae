#pragma once
#include "data_ranges.h"

template<class x>
struct random_access_range : range<x>
{
    using iterator = random_access_iterator<x>;

    //function from (iterator i) → same = {…}
    //function upto (iterator i) → same = {…}
    //operator [iterator i, iterator j] = from(i).upto(j+1);
    //operator [iterator i, iterator j) = from(i).upto(j);
    //
    //function from (integer n) = from(begin+n);
    //function upto (integer n) = upto(begin+n);
    //operator [integer b, integer e] = from(b).upto(e-b+1);
    //operator [integer b, integer e) = from(b).upto(e-b);
    //
    //operator [integer i] = (begin+i)::value;
    //operator (integer i) = (begin+i)::value;
};
