#pragma once
#include <span>
#include <cstdint>
namespace ae
{
    namespace system
    {
        // source: mutation in → byte [] ;
        // schema: mutation name -> name [] 
        // kind: mutation
        auto in() -> std::span<uint8_t>;
        // source: mutation out () ;
        // schema: mutation namepack 
        // kind: mutation
        auto out(std::span<uint8_t>) -> void;
        // source: mutation err () ;
        // schema: mutation namepack 
        // kind: mutation
        auto err(std::span<uint8_t>) -> void;
        // source: type app () {}
        // schema: type namepack 
        // kind: type
        template<typename type_window> struct app
        {
            // source: window window ;
            // schema: name name 
            // kind: expression
            window window;
        };
        void main_() { }
    }
}
