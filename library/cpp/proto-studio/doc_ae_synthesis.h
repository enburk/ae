#pragma once
#include "doc.h"
namespace doc::ae::synthesis
{
    /*
    array<entity> proceed (array<element> input)
    {
        array<entity> output;
        
        for (auto && e : input)
        {
            entity E;
            for (auto && t : e.head) E.head += *t;
            for (auto && t : e.tail) E.tail += *t;
            E.body = proceed(e.body);
            output += E;
        }

        return output;
    }
    */
}

