#pragma once
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
    // source: let N =
    // schema: let name =
    // kind: alias
    using N = natural;
    // source: let Z =
    // schema: let name =
    // kind: alias
    using Z = integer;
    // source: let Q =
    // schema: let name =
    // kind: alias
    using Q = rational;
    // source: let R =
    // schema: let name =
    // kind: alias
    using R = real;
    // source: let C =
    // schema: let name =
    // kind: alias
    using C = complex;
    // source: operator + () → N ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator +(N) -> N;
    // source: operator + () → Z ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator +(Z) -> Z;
    // source: operator + () → Q ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator +(Q) -> Q;
    // source: operator + () → R ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator +(R) -> R;
    // source: operator + () → C ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator +(C) -> C;
    // source: operator - () → Z ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator -(N) -> Z;
    // source: operator - () → Z ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator -(Z) -> Z;
    // source: operator - () → Q ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator -(Q) -> Q;
    // source: operator - () → R ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator -(R) -> R;
    // source: operator - () → C ;
    // schema: operator x () -> x 
    // kind: operator
    auto operator -(C) -> C;
    // source: operator () + () → N ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator +(N, N) -> N;
    // source: operator () - () → Z ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator -(N, N) -> Z;
    // source: operator () × () → N ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator *(N, N) -> N;
    // source: operator () / () → Q ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator /(N, N) -> Q;
    // source: operator () + () → Z ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator +(Z, Z) -> Z;
    // source: operator () - () → Z ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator -(Z, Z) -> Z;
    // source: operator () × () → Z ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator *(Z, Z) -> Z;
    // source: operator () / () → Q ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator /(Z, Z) -> Q;
    // source: operator () + () → Q ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator +(Q, Q) -> Q;
    // source: operator () - () → Q ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator -(Q, Q) -> Q;
    // source: operator () × () → Q ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator *(Q, Q) -> Q;
    // source: operator () / () → Q ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator /(Q, Q) -> Q;
    // source: operator () + () → R ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator +(R, R) -> R;
    // source: operator () - () → R ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator -(R, R) -> R;
    // source: operator () × () → R ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator *(R, R) -> R;
    // source: operator () / () → R ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator /(R, R) -> R;
    // source: operator () + () → C ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator +(C, C) -> C;
    // source: operator () - () → C ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator -(C, C) -> C;
    // source: operator () × () → C ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator *(C, C) -> C;
    // source: operator () / () → C ;
    // schema: operator () x () -> x 
    // kind: operator
    auto operator /(C, C) -> C;
    #include "C:\workn\ae\library\core\abc.ae.h++"
    void main_() { }
}
