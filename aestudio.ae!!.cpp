#include <span>
#include <cstdint>
namespace ae
{
    namespace data
    {
        template<class type_x, class type_f> auto name_gt_gt_gt_(type_x x, type_f f)
        {
            f(x);
        };
        template<class type_f, class type_x> auto name_lt_lt_lt_(type_f f, type_x x)
        {
            f(x);
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
    }
}
#include "D:\workn\ae\library\system.c++"
