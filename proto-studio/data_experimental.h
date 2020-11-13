
int function(auto data) {
}

auto sum (auto&&... ts) {
    return (ts + ...);
}

#include <concepts>

namespace test1
{
    struct X {};
    struct Derived : X {};
    struct Y { operator X() { return {}; }};
    struct Z {};

    auto foo(std::convertible_to<X> auto ... args) {}

    int test ()
    {
       int i{};
       X x{};
       Derived d{};
       Y y{};
       Z z{};

       foo(x, x, y, d); // should work
    // foo(x, x, y, d, z); // should not work due to unrelated z
    };
}
