#pragma once
#include <span>
#include <cstdint>
namespace ae
{
    namespace core
    {
        // source: type complex ;
        // schema: type name 
        // kind: type
        // kind: type
        // name: complex
        // args: ()
        // source: operator + () → N ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: N
        // args: (N )
        // source: operator + () → Z ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: Z
        // args: (Z )
        // source: operator + () → Q ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: Q
        // args: (Q )
        // source: operator + () → R ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: R
        // args: (R )
        // source: operator + () → C ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: C
        // args: (C )
        // source: operator - () → Z ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: Z
        // args: (N )
        // source: operator - () → Z ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: Z
        // args: (Z )
        // source: operator - () → Q ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: Q
        // args: (Q )
        // source: operator - () → R ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: R
        // args: (R )
        // source: operator - () → C ;
        // schema: operator x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: C
        // args: (C )
        // source: operator () + () → N ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: N
        // args: (N , N )
        // source: operator () - () → Z ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: Z
        // args: (N , N )
        // source: operator () × () → N ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: ae_times_
        // type: N
        // args: (N , N )
        // source: operator () / () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: /
        // type: Q
        // args: (N , N )
        // source: operator () + () → Z ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: Z
        // args: (Z , Z )
        // source: operator () - () → Z ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: Z
        // args: (Z , Z )
        // source: operator () × () → Z ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: ae_times_
        // type: Z
        // args: (Z , Z )
        // source: operator () / () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: /
        // type: Q
        // args: (Z , Z )
        // source: operator () + () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: Q
        // args: (Q , Q )
        // source: operator () - () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: Q
        // args: (Q , Q )
        // source: operator () × () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: ae_times_
        // type: Q
        // args: (Q , Q )
        // source: operator () / () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: /
        // type: Q
        // args: (Q , Q )
        // source: operator () + () → R ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: R
        // args: (R , R )
        // source: operator () - () → R ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: R
        // args: (R , R )
        // source: operator () × () → R ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: ae_times_
        // type: R
        // args: (R , R )
        // source: operator () / () → R ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: /
        // type: R
        // args: (R , R )
        // source: operator () + () → C ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: +
        // type: C
        // args: (C , C )
        // source: operator () - () → C ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: -
        // type: C
        // args: (C , C )
        // source: operator () × () → C ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: ae_times_
        // type: C
        // args: (C , C )
        // source: operator () / () → C ;
        // schema: operator () x () -> x 
        // kind: operator
        // kind: operator
        // name: /
        // type: C
        // args: (C , C )
        // source: type value.of () {}
        // schema: type namepack 
        // kind: type
        // kind: type
        // name: value.of
        // args: ( x)
        {
            // source: function at () → x ;
            // schema: function namepack -> name 
            // kind: function
            auto at(address ) const  -> x;
            // kind: function
            // name: at
            // type: x
            // args: (address )
            // source: ;
            // schema: 
            // kind: 
            // args: ()
        }
        // source: type pointer () {}
        // schema: type namepack 
        // kind: type
        // kind: type
        // name: pointer
        // args: ( x)
        {
            // source: address address ;
            // schema: name name 
            // kind: expression
            // kind: expression
            // args: ()
            address address;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: property value → x =
            // schema: property name -> name =
            // kind: property
            // kind: property
            // name: value
            // type: x
            // args: ()
            value.of(x)::at(address);
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: property value ← x =
            // schema: property name <- name =
            // kind: property
            // kind: property
            // name: value
            // type: x
            // args: ()
            value.of(x)::at(address) ← value;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
        }
        void main_()
        {
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: type real subset.of complex ;
            // schema: type name subset.of name 
            // kind: type subset.of
            // kind: type subset.of
            // name: real
            // type: complex
            // args: ()
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: type rational subset.of real ;
            // schema: type name subset.of name 
            // kind: type subset.of
            // kind: type subset.of
            // name: rational
            // type: real
            // args: ()
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: type integer subset.of rational ;
            // schema: type name subset.of name 
            // kind: type subset.of
            // kind: type subset.of
            // name: integer
            // type: rational
            // args: ()
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: type natural subset.of integer ;
            // schema: type name subset.of name 
            // kind: type subset.of
            // kind: type subset.of
            // name: natural
            // type: integer
            // args: ()
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: let N =
            // schema: let name =
            // kind: alias
            // kind: alias
            // name: N
            // args: ()
            natural;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: let Z =
            // schema: let name =
            // kind: alias
            // kind: alias
            // name: Z
            // args: ()
            integer;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: let Q =
            // schema: let name =
            // kind: alias
            // kind: alias
            // name: Q
            // args: ()
            rational;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: let R =
            // schema: let name =
            // kind: alias
            // kind: alias
            // name: R
            // args: ()
            real;
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: let C =
            // schema: let name =
            // kind: alias
            // kind: alias
            // name: C
            // args: ()
            complex;
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
            // source: ;
            // schema: 
            // kind: 
            // args: ()
            // source: type address subset.of natural ;
            // schema: type name subset.of name 
            // kind: type subset.of
            // kind: type subset.of
            // name: address
            // type: natural
            // args: ()
            // source: ;
            // schema: 
            // kind: 
            // args: ()
        }
    }
}
