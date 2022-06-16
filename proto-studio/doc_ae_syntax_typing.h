#pragma once
#include <map>
#include "doc_ae_syntax_visitor.h"
#include "doc_ae_syntax_expressions.h"
namespace doc::ae::syntax::typing
{
    void parse (array<statement> statements, scope& scope, report& log)
    {
        visitor visitor;
        visitor.on_expression = [&](expression& e)
        {
            expressions::parse(e, scope, log);
        };
        visitor.pass(statements);

    }
}


