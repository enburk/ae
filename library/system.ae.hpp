#pragma once
#include <span>
#include <cstdint>
namespace ae
{
    namespace system
    {
        // source: mutation out ()
        // schema: mutation namepack
        // kind: mutation
        auto out(std::span<uint8_t> data) -> void;
        // kind: mutation
        // name: out
        // type: void
        // args: (std::span<uint8_t> data)
        // source: mutation err ()
        // schema: mutation namepack
        // kind: mutation
        auto err(std::span<uint8_t> data) -> void;
        // kind: mutation
        // name: err
        // type: void
        // args: (std::span<uint8_t> data)
        void main_()
        {
            // source: 
            // schema: 
            // kind: 
            // args: ()
            // source: 
            // schema: 
            // kind: 
            // args: ()
        }
    }
}
