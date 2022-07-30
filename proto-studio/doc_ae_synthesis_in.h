#pragma once
#include "doc_text_text.h"
#include "doc_ae_synthesis.h"
namespace doc::ae::synthesis
{
    auto in (syntax::expression const&) -> expression;

    auto in (syntax::terminal const& x)
    {
        terminal y; if (x.token) {
        y.text = x.token->text;
        y.kind = x.token->kind; }
        y.type = x.type;
        return y;
    }

    auto in (syntax::brackets const& x)
    {
        brackets y;
        y.opening = 
        x.opening ?
        x.opening->text : "(";
        y.closing =
        x.closing ?
        x.closing->text : ")";
        for (auto& e: x.list)
        y.list += in(e);
        return y;
    }

    auto in (syntax::nameunit const& x)
    {
        nameunit y;
        y.identifier =
        x.identifier->text;
        for (auto& args: x.argss)
        y.argss += in(args);
        return y;
    }

    auto in (syntax::namepack const& x)
    {
        namepack y;
        for (auto& name: x.names)
        y.names += in(name);
        y.type = x.type;
        return y;
    }

    auto in (syntax::expression const& x) -> expression
    {
        expression y;

        std::visit(overloaded
        {
            [&](syntax::terminal v) { y = expression{in(v)}; },
            [&](syntax::namepack v) { y = expression{in(v)}; },
            [&](syntax::brackets v) { y = expression{in(v)}; },
            [&](syntax::operands v) {

                operands oo;
                for (auto& o: v.list)
                oo.list += in(o);
                y = expression{oo};
            },
        },
        x.variant);

        return y;
    }

    auto in (syntax::parameter const& x)
    {
        parameter y;
        y.name = x.name? x.name->text : "";
        y.typexpr = in(x.typexpr);
        y.value = in(x.value);
        y.type = x.type;
        return y;
    }

    auto in (syntax::statement const& x) -> statement
    {
        statement y;

        y.kind = x.kind;
        y.type = x.type;

        for (auto& name: x.names)
        y.names.list += name->text;

        for (auto& arg: x.args.list)
        y.args.list += in(arg);

        y.expr = in(x.expr);
        y.typexpr = in(
        x.typexpr);

        y.schema = x.schema;
        y.source = x.source;

        y.body.reserve(x.body.size());
        for (auto& st: x.body)
        y.body += in(st);

        return y;
    }
}

