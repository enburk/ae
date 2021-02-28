#pragma once
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct visitor
    {
        std::function<void(namepack   &)> on_namepack   = [](namepack   &){};
        std::function<void(expression &)> on_expression = [](expression &){};
        std::function<void(statement  &)> on_statement  = [](statement  &){};

        void pass (array<statement> & statements)
        {
            for (auto & st : statements)
            {
                on_statement(st);
                on_namepack(st.type);

                for (auto & arg : st.args.list)
                {
                    on_namepack(arg.type);
                    pass(arg.value);
                }

                pass(st.expr);
                pass(st.body);
            }
        }

        void pass (expression & e)
        {
            on_expression(e);

            std::visit(overloaded
            {
                [&](terminal & v) {},
                [&](namepack & v) { on_namepack(v); },
                [&](brackets & v) { for (auto & o : v.list) pass(o); },
                [&](operands & v) { for (auto & o : v.list) pass(o); },
            },
            e.variant);
        }
    };
}

