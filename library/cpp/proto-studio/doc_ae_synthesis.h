#pragma once
#include "doc.h"
namespace doc::ae::synthesis
{
    using namespace doc::ae::syntax;

    array<entity> proceed (array<statement> input)
    {
        array<entity> output;
        
        for (auto && e : input)
        {
            entity E;

            //if (e.opening) E.head += *e.opening;
            //
            //E.body += proceed(e.elements);
            //
            //if (e.closing) E.tail += *e.closing;

            output += E;
        }

        return output;
    }
}

