#pragma once
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct visitor
    {
        std::function<void(statement  &)> in_statement;
        std::function<void(statement  &)> on_statement;
        std::function<void(namepack   &, statement*)> in_namepack;
        std::function<void(namepack   &, statement*)> on_namepack;
        std::function<void(expression &, statement*)> in_expression;
        std::function<void(expression &, statement*)> on_expression;

        statement* scope = nullptr;

        void pass (statement s, auto& cancel)
        {
            if (cancel)
                return;

            if (
            in_statement)
            in_statement(s);

            scope = &s;

            for (auto& arg : s.args.list)
            {
                pass(arg.typexpr);
                pass(arg.value);
            }

            pass(s.expr);
            pass(s.typexpr);

            for (auto& ss: s.body)
            {
                pass(ss, cancel);
            }

            if (
            in_statement)
            in_statement(s);
        }

        void pass (namepack& n)
        {
            if (n.names.empty())
                return;

            if (
            in_namepack)
            in_namepack(n,
                scope);

            for (auto& name: n.names)
            for (auto& args: name.argss)
            for (auto& expr: args.list)
            pass(expr);

            if (
            on_namepack)
            on_namepack(n,
                scope);
        }

        void pass (expression& e)
        {
            if (
            in_expression)
            in_expression(e,
                scope);

            std::visit(overloaded
            {
                [&](terminal & v) {},
                [&](namepack & v) { pass(v); },
                [&](brackets & v) { for (auto& o: v.list) pass(o); },
                [&](operands & v) { for (auto& o: v.list) pass(o); },
            },
            e.variant);

            if (
            on_expression)
            on_expression(e,
                scope);
        }
    };
}

