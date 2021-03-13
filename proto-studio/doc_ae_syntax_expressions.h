#pragma once
#include "doc_ae_syntax_scopes.h"
namespace doc::ae::syntax::expressions
{
    void parse (expression& e, scopes& scopes, report& log)
    {
        std::visit(overloaded
        {
            [&](terminal & v)
            {
                if (!v.token) return;
                e.typeline = v.token->kind;
                v.token->info = e.typeline;
            },
            [&](namepack & v)
            {
            },
            [&](brackets & v)
            {
                //for (auto & o : v.list) pass(o);
            },
            [&](operands & v)
            {
                //for (auto & o : v.list) pass(o);
            },
        },
        e.variant);
    }
}

