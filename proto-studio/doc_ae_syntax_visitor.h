#pragma once
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct visitor
    {
        std::function<void(namepack   &)> on_namepack   = [](namepack   &){};
        std::function<void(expression &)> on_expression = [](expression &){};
        std::function<void(statement  &)> on_statement  = [](statement  &){};

        scope* scope = nullptr;

        void pass (array<statement>& statements)
        {
            for (auto & st : statements)
            {
                on_statement(st);

                scope = st.scope;

                for (auto & arg : st.args.list)
                {
                    pass(arg.type);
                    pass(arg.value);
                }

                pass(st.type);
                pass(st.expr);
                pass(st.body);
            }
        }

        void pass (namepack& n)
        {
            on_namepack(n);

            for (auto& name: n.names)
            for (auto& arg: name.args)
            for (auto& expr: arg.list)
            pass(expr);
        }

        void pass (expression& e)
        {
            e.scope = scope;

            on_expression(e);

            std::visit(overloaded
            {
                [&](terminal & v) {},
                [&](namepack & v) { pass(v); },
                [&](brackets & v) { for (auto& o: v.list) pass(o); },
                [&](operands & v) { for (auto& o: v.list) pass(o); },
            },
            e.variant);
        }
    };
}

