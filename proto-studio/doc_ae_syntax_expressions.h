#pragma once
#include "doc_ae_syntax.h"
namespace doc::ae::syntax::expressions
{
    void parse (expression& e, statement* scope, report& log)
    {
        std::visit(overloaded
        {
            [&](terminal & v)
            {
                if (not v.token) return;
                str kind = v.token->kind;
                if (kind == "number")
                {
                    e.type.name = "integer";
                }
                else
                if (kind == "literal")
                {
                    e.type.name = "byte[]";
                }
                else log.error(v.token,
                "unknown kind of terminal: "
                    + kind);

                v.token->info =
                e.type.name;
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

