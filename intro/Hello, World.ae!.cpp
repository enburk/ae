#pragma once
#include <span>
#include <cstdint>
namespace ae
{
    void main_()
    {
        // source:  :: system :: out ()
        // schema: namepack
        // kind: expression
        system::out(std::span((uint8_t*)"Hello, World!",15));
    }
}
