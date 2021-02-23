#pragma once
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct visitor
    {
        std::function<void(namepack&)> on_name;

        void pass (array<statement> & statmenets)
        {
        }
    };
}

