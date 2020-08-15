#include <iostream>

void type_system::out (std::span<uint8_t> data)
{
    std::cout.write((char*)(data.data()), data.size());
    std::cout.write("\n", 1);
    std::cout.flush();
}




