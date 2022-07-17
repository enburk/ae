#pragma once
#include "doc_ae_synthesis_visitor.h"
namespace doc::ae::synthesis
{
    void proceed (statement& st, std::atomic<bool>& cancel)
    {
        visitor visitor;
        visitor.on_statement = [&](statement& s)
        {
        };
        visitor.on_namepack = [&](namepack& pack, statement* scope)
        {
        };
        visitor.on_expression = [&](expression& e, statement* scope)
        {
        };
        visitor.pass(st, cancel);

    }
}

