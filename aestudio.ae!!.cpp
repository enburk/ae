#pragma once
#include <span>
#include <cstdint>
#include "C:\workn\ae\library\core.ae.hpp"
#include "C:\workn\ae\library\gui.ae.hpp"
#include "C:\workn\ae\library\system.ae.hpp"
namespace ae
{
    namespace ae
    {
        // source: let gui =
        // schema: let name =
        // kind: alias
        using gui = gui;
        // source: let sys =
        // schema: let name =
        // kind: alias
        using sys = system;
        // source: let core =
        // schema: let name =
        // kind: alias
        using core = core;
        // source: type window extends gui :: widget {}
        // schema: type name extends namepack 
        // kind: type
        struct window
        {
            // source: 
            // schema: 
            // kind: empty
        };
        void main_()
        {
            // source: sys :: app () app ;
            // schema: namepack name 
            // kind: expression
            sys::app(window) app;
        }
    }
}
#include "C:\workn\ae\library\system.ae.c++"
