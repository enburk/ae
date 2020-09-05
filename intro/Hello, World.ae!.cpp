#include <span>
#include <cstdint>
namespace ae
{
    namespace math
    {
        double ae__sqrt_(double x);
        constexpr auto golden_ratio =
        {
            ( =, (, /, 2 );
        }
    }
    namespace data
    {
        template<class type_x, class type_f> auto ae__gt__gt__gt_(type_x x, type_f f)
        {
            f(x);
        };
        template<class type_f, class type_x> auto ae__lt__lt__lt_(type_f f, type_x x)
        {
            f(x);
        };
        template<typename value_type> struct array
        {
            struct type_detail
            {
                unsigned size, capacity;
                std::span<value_type> data;
            };
            type_detail detail;
            auto size()
            {
                detail::size;
            };
            auto size(unsigned n)
            {
            };
            auto capacity()
            {
                detail::capacity;
            };
            auto capacity(unsigned n)
            {
            };
        };
        template<typename x> struct cow_array_on_heap
        {
        };
    }
    namespace gui
    {
        struct screen
        {
            struct type_size
            {
                int x, y;
            };
            type_size size;
        };
        struct window
        {
            void create();
        };
    }
    namespace system
    {
        void out(std::span<uint8_t> data);
        void err(std::span<uint8_t> data);
        gui::window window;
        gui::screen screen;
        std::span<gui::screen> screens;
    }
    void main_()
    {
        data::ae__gt__gt__gt_ ( std::span<uint8_t>((uint8_t*)("Hello, World!"), 13), system::out );
    }
}
#include "D:\workn\ae\library\math.c++" #include "D:\workn\ae\library\system.c++"
