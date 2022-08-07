#pragma once
#include <span>
#include <cstdint>
namespace ae
{
    namespace abc
    {
        // source: type complex ;
        // schema: type name 
        // kind: type
        struct complex;
        // source: type real subset.of complex ;
        // schema: type name subset.of name 
        // kind: type
        struct real;
        // source: type rational subset.of real ;
        // schema: type name subset.of name 
        // kind: type
        struct rational;
        // source: type integer subset.of rational ;
        // schema: type name subset.of name 
        // kind: type
        struct integer;
        // source: type natural subset.of integer ;
        // schema: type name subset.of name 
        // kind: type
        struct natural;
        // source: operator + () → N ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator + () → Z ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator + () → Q ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator + () → R ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator + () → C ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator - () → Z ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator - () → Z ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator - () → Q ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator - () → R ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator - () → C ;
        // schema: operator x () -> x 
        // kind: operator
        // source: operator () + () → N ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () - () → Z ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () × () → N ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () / () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () + () → Z ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () - () → Z ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () × () → Z ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () / () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () + () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () - () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () × () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () / () → Q ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () + () → R ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () - () → R ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () × () → R ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () / () → R ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () + () → C ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () - () → C ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () × () → C ;
        // schema: operator () x () -> x 
        // kind: operator
        // source: operator () / () → C ;
        // schema: operator () x () -> x 
        // kind: operator
        void main_()
        {
            // source: let N =
            // schema: let name =
            // kind: alias
            // source: ;
            // schema: 
            // kind: 
            // source: let Z =
            // schema: let name =
            // kind: alias
            // source: ;
            // schema: 
            // kind: 
            // source: let Q =
            // schema: let name =
            // kind: alias
            // source: ;
            // schema: 
            // kind: 
            // source: let R =
            // schema: let name =
            // kind: alias
            // source: ;
            // schema: 
            // kind: 
            // source: let C =
            // schema: let name =
            // kind: alias
            // source: ;
            // schema: 
            // kind: 
        }
    }
}
#include "C:\workn\ae\library\core\abc.ae.h++"
