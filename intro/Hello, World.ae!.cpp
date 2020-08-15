#include <span>
#include <cstdint>
template<class type_x, class type_f> auto name_gt_gt_gt_(type_x x, type_f f)
{
    ( f(x) );
};
template<class type_f, class type_x> auto name_lt_lt_lt_(type_f f, type_x x)
{
    ( f(x) );
};
struct type_system
{
    static void out(std::span<uint8_t> data);
    static void err(std::span<uint8_t> data);
};
#include <iostream>
int main (int argc_, char *argv_[])
{
    type_system system;
    name_gt_gt_gt_ ( std::span<uint8_t>((uint8_t*)("Hello, World!"), 13) , type_system::out );
    std::cout << "\n";
    std::cout << "press Enter to close...";
    std::cout << std::endl;
    std::cin.get();
}
#include <iostream>

void type_system::out (std::span<uint8_t> data)
{
    std::cout.write((char*)(data.data()), data.size());
    std::cout.write("\n", 1);
    std::cout.flush();
}






