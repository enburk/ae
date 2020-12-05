template<class x>
struct forward_iterator
{
    auto operator *  () /***/&  -> x /***/& ;
    auto operator *  () /***/&& -> x /***/&&;
    auto operator *  () const&  -> x const& ;
    auto operator *  () const&& -> x const&&;
    void operator ++ (int); // postfix
    void operator ++ ();    // prefix
};

template<class x>
struct bidirectional_iterator : forward_iterator<x>
{
    void operator -- (int); // postfix
    void operator -- ();    // prefix
};

template<class x>
struct random_access_iterator : bidirectional_iterator<x> // , ordered
{
    #define random_access_iterator_impl      \
    void operator ++ ()    { (*this) += 1; } \
    void operator -- ()    { (*this) -= 1; } \
    void operator ++ (int) { (*this) += 1; } \
    void operator -- (int) { (*this) -= 1; } \
    auto operator +  (int n) { auto i = *this; i += n; return i; } \
    auto operator -  (int n) { auto i = *this; i -= n; return i; }
    void operator += (int n);
    void operator -= (int n);
    auto operator -  (random_access_iterator i) -> int;
    random_access_iterator_impl;
};

template<class x>
struct contiguous_iterator : random_access_iterator<x>
{
    x* pointer;
    contiguous_iterator(x* p) : pointer(p) {}
    void operator += (int n) { pointer += n; }
    void operator -= (int n) { pointer -= n; }
    auto operator *  () /***/  -> x /***/&  { return /*******/(*pointer); }
    auto operator *  () const  -> x const&  { return /*******/(*pointer); }
    auto operator -  (contiguous_iterator i) { return (pointer - i.pointer); }
    auto operator != (contiguous_iterator i) { return (pointer != i.pointer); }
    auto operator <=>(contiguous_iterator i) { return (pointer <=> i.pointer); }
    random_access_iterator_impl;
};

struct integer_iterator : random_access_iterator<int>
{
    int value;
    integer_iterator (int v) : value(v) {}
    void operator += (int n) { value += n; }
    void operator -= (int n) { value -= n; }
    auto operator *  () /***/ -> int /***/& { return value; }
    auto operator *  () const -> int const& { return value; }
    auto operator -  (integer_iterator i) { return value - i.value; }
    auto operator != (integer_iterator i) { return value != i.value; }
    auto operator <=>(integer_iterator i) { return value <=> i.value; }
    random_access_iterator_impl;
};

