#pragma once
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::synthesis
{
    using namespace doc::ae::syntax;

    array<entity> proceed (const array<statement> & input);

    array<entity> add (const loop_for    & s) { array<entity> output; return output; }
    array<entity> add (const loop_while  & s) { array<entity> output; return output; }
    array<entity> add (const expression  & s) { array<entity> output; return output; }
    array<entity> add (const conditional & s) { array<entity> output; return output; }
    array<entity> add (const declaration & s) { array<entity> output; return output; }
    array<entity> add (const subroutine  & s) { array<entity> output; return output; }
    array<entity> add (const pragma      & s) { array<entity> output; return output; }

    array<entity> proceed (const array<statement> & input)
    {
        array<entity> output;

        for (auto && s : input)
        {
            array<entity> ee;

            std::visit(aux::overloaded
            {
                [&](loop_for    s) { ee += add(s); },
                [&](loop_while  s) { ee += add(s); },
                [&](expression  s) { ee += add(s); },
                [&](conditional s) { ee += add(s); },
                [&](declaration s) { ee += add(s); },
                [&](subroutine  s) { ee += add(s); },
                [&](pragma      s) { ee += add(s); },
            },
            s.variant);

            output += ee;
        }

        return output;
    }

    array<entity> proceed (const syntax::analysis::data & data)
    {
        array<entity> output;

        entity main; main.head += token{"int main()"};
        
        for (auto && s : data.statements)
        {
            array<entity> ee;

            std::visit(aux::overloaded
            {
                [&](loop_for    s) { main.body += add(s); },
                [&](loop_while  s) { main.body += add(s); },
                [&](expression  s) { main.body += add(s); },
                [&](conditional s) { main.body += add(s); },
                [&](declaration s)
                {
                    //if (s.kind == "variable") main.body += add(s);
                    //if (s.kind == "singleton") { output += add(s);
                    //main.body += entity{.head = token{s.names[0]->text + " " + s.names[0]->text}};
                    //}
                },
                [&](subroutine  s) { output += add(s); },
                [&](pragma      s) { output += add(s); },
            },
            s.variant);

            output += ee;
        }

        output += main;

        return output;
    }
}

