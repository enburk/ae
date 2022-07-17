#pragma once
#include <span>
#include <cstdint>
#include "C:\workn\ae\library\system.ae.hpp"
namespace ae
{
    namespace ae
    {
        void main_()
        {
            // source: :: system :: out "Hello, World!"
            // schema: namepack literal
            // kind: 
            // args: ()
            system::out(std::span((uint8_t*)"Hello, World!",15));
        }
    }
}
#include "C:\workn\ae\library\system.ae.c++"
