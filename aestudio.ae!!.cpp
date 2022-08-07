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
        void main_()
        {
            // source: let gui =
            // schema: let name =
            // kind: alias
            // kind: alias
            // name: gui
            // args: ()
            gui;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: let sys =
            // schema: let name =
            // kind: alias
            // kind: alias
            // name: sys
            // args: ()
            system;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: let core =
            // schema: let name =
            // kind: alias
            // kind: alias
            // name: core
            // args: ()
            core;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: type window extends gui :: widget {}
            // schema: type name extends namepack 
            // kind: type extends
            // kind: type extends
            // name: window
            // type: gui::widget
            // args: ()
            {
                // source: 
                // schema: 
                // kind: 
                // args: ()
            }
            // source: sys :: app () app ;
            // schema: namepack name 
            // kind: expression
            // kind: expression
            // args: ()
            sys::app(window) app;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
        }
    }
}
#include "C:\workn\ae\library\system.ae.c++"
