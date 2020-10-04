#pragma once
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::syntax::analysis
{
    void pass2 (data & record, scope* scope, loop_for & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, scope* scope, loop_while & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, scope* scope, expression & e)
    {
        std::visit(aux::overloaded
        {
            [&](number  &v) {},
            [&](symbol  &v) {},
            [&](literal &v) {},
            [&](named_pack &v)
            {

            },
            [&](operation &v)
            {
                for (auto & o : v.operands)
                    pass2(record, scope, o);

                //deque<expression> deque(v.operands);
                //
                //v.operands.clear();
                //v.operands += deque.front();
                //deque.pop_front();

                if (v.operands.size() == 3 and
                    aux::got<symbol>(v.operands[1].variant)) { v.title =
                    std::get<symbol>(v.operands[1].variant).token;
                    v.operands.from(1).upto(2).erase();
                }
            },
            [&](brackets &v)
            {
            },
            [&](expression_if &v)
            {
                for (auto & e : v.condition) pass2(record, scope, e);
                for (auto & e : v.then_body) pass2(record, scope, e);
                for (auto & e : v.else_body) pass2(record, scope, e);
            },
            [&](expression_for &v)
            {
                for (auto & e : v.range) pass2(record, scope, e);
            },
            [&](lambda &v)
            {
                pass2(record, v.body);
            },
        },
        e.variant);
    }
    void pass2 (data & record, scope* scope, conditional & s)
    {
        pass2(record, s.then_body);
        pass2(record, s.else_body);
    }
    void pass2 (data & record, scope* scope, declaration & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, scope* scope, definition & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, scope* scope, subroutine & s)
    {
        pass2(record, s.body);
    }
    void pass2 (data & record, scope* scope, directive & s)
    {
    }
    void pass2 (data & record, scope* scope, pragma & s)
    {
    }
}

