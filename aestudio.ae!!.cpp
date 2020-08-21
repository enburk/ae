#include <span>
#include <cstdint>
namespace ae
{
    template<class type_x, class type_f> auto name_gt_gt_gt_(type_x x, type_f f)
    {
        ( f(x) );
    };
    template<class type_f, class type_x> auto name_lt_lt_lt_(type_f f, type_x x)
    {
        ( f(x) );
    };
    namespace system
    {
        static void out(std::span<uint8_t> data);
        static void err(std::span<uint8_t> data);
    }
    void main_()
    {
        : ( studio , system::app{} );
    }
}
#include "D:\workn\ae\library\system.c++"
