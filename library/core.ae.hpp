#pragma once
namespace core
{
    #include "C:\workn\ae\library\core\abc.ae.hpp"
    // source: type address subset.of natural ;
    // schema: type name subset.of name 
    // kind: type
    struct address;
    // source: type value.of () {}
    // schema: type namepack 
    // kind: type
    template<typename type_x> struct value_of
    {
        // source: function at () → x ;
        // schema: function namepack -> name 
        // kind: function
        auto at(address) const  -> x;
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
        // source: property value → x =
        // schema: property name -> name =
        // kind: property
        {
            // source: 
            // schema: 
            // kind: return
            return value_of(x)::at(address);
        }
        // source: property value ← x =
        // schema: property name <- name =
        // kind: property
        {
            // source: 
            // schema: 
            // kind: return
            return value_of(x)::at(address) ← value;
        }
    };
    void main_()
    {
        // source: import () ;
        // schema: namepack 
        // kind: expression
        import(abc);
    }
}
