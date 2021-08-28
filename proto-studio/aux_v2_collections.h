#pragma once
#include "aux_v2_ranges.h"

template<class X> struct
contiguous_collection_range_
{
    const X& host;
    using iterator = typename X::iterator_;
    using sentinel = typename X::sentinel_;
    using iterator_ = typename X::iterator_;
    using sentinel_ = typename X::sentinel_;
    using value_type = typename X::value_type;
    using range_type = contiguous_collection_range_;
    auto range (iterator i, iterator j) { return range_type{host, i, j}; }

    iterator  begin_;
    sentinel  end_;
    iterator  begin () { return begin_; }
    sentinel  end   () { return end_;   }
    iterator_ begin () const { return begin_; }
    sentinel_ end   () const { return end_;   }
    contiguous_range_impl(value_type);

    auto upto (int n) { return range(begin(), host.clip(host.begin() + n)); }
    auto span (int n) { return range(begin(), host.clip(     begin() + n)); }

    auto offset () { return (int)(begin() - host.begin()); }

    bool operator != (range_type r) { return !(*this == r); }
    bool operator == (range_type r) { return
        std::equal(begin(), end(),
            r.begin(), r.end(); }
    auto operator <=> (range_type r) { return
        std::lexicographical_compare_three_way(begin(), end(),
            r.begin(), r.end());
    }

    #include "aux_v2_algo_random.h"
};

template<class X> struct
contiguous_collection_range
{
    X& host;
    using iterator = typename X::iterator;
    using sentinel = typename X::sentinel;
    using iterator_ = typename X::iterator_;
    using sentinel_ = typename X::sentinel_;
    using value_type = typename X::value_type;
    using range_type = contiguous_collection_range;
    auto range (iterator  i, iterator  j) { return range_type {host, i, j}; }

    iterator  begin_;
    sentinel  end_;
    iterator  begin () { return begin_; }
    sentinel  end   () { return end_;   }
    iterator_ begin () const { return begin_; }
    sentinel_ end   () const { return end_;   }
    contiguous_range_impl(value_type);

    auto upto (int n) { return range(begin(), host.clip(host.begin() + n)); }
    auto span (int n) { return range(begin(), host.clip(     begin() + n)); }

    auto offset () const { return (int)(begin() - host.begin()); }

    void insert (iterator i, range_type r) { host.insert(i, r); }
    void insert (iterator i, value_type e) { host.insert(i, std::move(e)); }
    void erase  () { host.erase(begin(), end()); end_ = begin_; }

    //void operator = (range_type       r) { replace_by(r); }
    //void operator = (input_range auto r) { replace_by(r); }
    //void operator = (const char *     r) { replace_by(r); }

    bool operator != (range_type r) { return !(*this == r); }
    bool operator == (range_type r) { return
        std::equal(begin(), end(),
            r.begin(), r.end());
    }
    auto operator <=> (range_type r) { return
        std::lexicographical_compare_three_way(begin(), end(),
            r.begin(), r.end());
    }

    #include "aux_v2_algo_random.h"
    #include "aux_v2_algo_resizing.h"
};
