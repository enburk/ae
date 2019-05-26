#include <iostream> // pragma // #include <iostream>
#include "sys.h" // pragma // #include "sys.h"
void sys :: window :: on :: size ( ) //  // name name
{
}
void sys :: window :: on :: draw ( int x , int y , int w , int h ) //  // name name
{
    for ( auto & c : sys :: window :: image ) c = 0xFFFF0000; //  // for () name = number
}
