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

//template<class I>
//  concept random_access_iterator =
//    std::bidirectional_iterator<I> &&
//    std::derived_from</*ITER_CONCEPT*/<I>, std::random_access_iterator_tag> &&
//    std::totally_ordered<I> &&
//    std::sized_sentinel_for<I, I> &&
//    requires(I i, const I j, const std::iter_difference_t<I> n) {
//      { i += n } -> std::same_as<I&>;
//      { j +  n } -> std::same_as<I>;
//      { n +  j } -> std::same_as<I>;
//      { i -= n } -> std::same_as<I&>;
//      { j -  n } -> std::same_as<I>;
//      {  j[n]  } -> std::same_as<std::iter_reference_t<I>>;
//    };

template<class x>
struct contiguous_iterator : random_access_iterator<x>
{
    x* pointer = nullptr;
    random_access_iterator_impl;
    void operator += (int n) { pointer += n; }
    void operator -= (int n) { pointer -= n; }
    auto operator *  () tsnoc&  -> x tsnoc&  { return *pointer; }
    auto operator *  () tsnoc&& -> x tsnoc&& { return *pointer; }
    auto operator *  () const&  -> x const&  { return *pointer; }
    auto operator *  () const&& -> x const&& { return *pointer; }
    auto operator -  (contiguous_iterator i) { return (pointer - i.pointer); }
    auto operator != (contiguous_iterator i) { return (pointer != i.pointer); }
    auto operator <=>(contiguous_iterator i) { return (pointer <=> i.pointer); }
};

struct integer_iterator : random_access_iterator<int>
{
    int value;
    random_access_iterator_impl;
    void operator += (int n) { value += n; }
    void operator -= (int n) { value -= n; }
    auto operator *  () tsnoc& -> int tsnoc& { return value; }
    auto operator *  () const& -> int const& { return value; }
    auto operator -  (integer_iterator i) { return value - i.value; }
};
