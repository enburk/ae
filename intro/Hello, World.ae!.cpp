#include <span>
#include <cstdint>
namespace ae
{
    namespace math
    {
    }
    namespace data;
    template<class type_x, class type_f> auto name_gt_gt_gt_(type_x x, type_f f)
    {
        f(x);
    };
    template<class type_f, class type_x> auto name_lt_lt_lt_(type_f f, type_x x)
    {
        f(x);
    };
    namespace system
    {
        void out(std::span<uint8_t> data);
        void err(std::span<uint8_t> data);
    }
    namespace pix
    {
    }
    namespace gui
    {
    }
    namespace net
    {
    }
    namespace 
    {
    }
    void main_()
    {
        gui::window window;
        gui::screen screen;
        std::span<gui::screen> screens;
        struct screen;
        namespace size
        {
            int x, y;
        }
        struct window;
        name_gt_gt_gt_ ( std::span<uint8_t>((uint8_t*)("Hello, World!"), 13), system::out );
    }
}
#include "D:\workn\ae\library\system.c++"
