#pragma once
#include <span>
#include <cstdint>
namespace ae
{
#include "C:\workn\ae\library\core\abc.ae.hpp"
    namespace core
    {
        // source: type address subset.of natural ;
        // schema: type name subset.of name 
        // kind: type
        struct address;
        // source: type value.of () {}
        // schema: type namepack 
        // kind: type
        template<typename type_x> struct value.of
        {
            // source: function at () → x ;
            // schema: function namepack -> name 
            // kind: function
            auto at(address ) const  -> x;
        };
        // source: type pointer () {}
        // schema: type namepack 
        // kind: type
        template<typename type_x> struct pointer
        {
            // source: address address ;
            // schema: name name 
            // kind: expression
            address address;
            // source: ;
            // schema: 
            // kind: 
            // source: property value → x =
            // schema: property name -> name =
            // kind: property
            {
                // source: 
                // schema: 
                // kind: return
                return value.of(x)::at(address);
            }
            // source: ;
            // schema: 
            // kind: 
            // source: property value ← x =
            // schema: property name <- name =
            // kind: property
            {
                // source: 
                // schema: 
                // kind: return
                return value.of(x)::at(address) ← value;
            }
            // source: ;
            // schema: 
            // kind: 
        };
        void main_()
        {
            // source: import () ;
            // schema: namepack 
            // kind: expression
            import(abc);
            // source: ;
            // schema: 
            // kind: 
        }
    }
}
