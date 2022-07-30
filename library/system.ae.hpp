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
        // kind: mutation
        // name: in
        // type: std::span<uint8_t>
        // args: ()
        // source: mutation out () ;
        // schema: mutation namepack 
        // kind: mutation
        auto out(std::span<uint8_t> ) -> void;
        // kind: mutation
        // name: out
        // type: void
        // args: (std::span<uint8_t> )
        // source: mutation err () ;
        // schema: mutation namepack 
        // kind: mutation
        auto err(std::span<uint8_t> ) -> void;
        // kind: mutation
        // name: err
        // type: void
        // args: (std::span<uint8_t> )
        void main_()
        {
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: ;
            // schema: 
            // kind: 
            // args: ()
        }
    }
}
