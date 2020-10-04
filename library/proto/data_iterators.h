#pragma once
#define tsnoc // anti const

template<class x>
struct forward_iterator
{
    auto operator *  () tsnoc&  -> x tsnoc& ;
    auto operator *  () tsnoc&& -> x tsnoc&&;
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
    x* pointer = nullptr;
    random_access_iterator_impl;
    auto operator *  () tsnoc&  { return *pointer; }
    auto operator *  () tsnoc&& { return *pointer; }
    auto operator *  () const&  { return *pointer; }
    auto operator *  () const&& { return *pointer; }
    void operator += (int n) { pointer += n; }
    void operator -= (int n) { pointer -= n; }
    auto operator -  (contiguous_iterator i) { return (pointer - i.pointer); }
    auto operator != (contiguous_iterator i) { return (pointer != i.pointer); }
    auto operator <=> (contiguous_iterator i) { return (pointer <=> i.pointer); }
};

struct integer_iterator : random_access_iterator<int>
{
    int value;
    random_access_iterator_impl;
    auto operator *  () tsnoc&  { return value; }
    auto operator *  () tsnoc&& { return value; }
    auto operator *  () const&  { return value; }
    auto operator *  () const&& { return value; }
    void operator += (int n) { value += n; }
    void operator -= (int n) { value -= n; }
    auto operator -  (integer_iterator i) { return value - i.value; }
};
