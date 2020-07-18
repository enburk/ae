#pragma once
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::synthesis
{
    using namespace doc::ae::syntax;

    struct context
    {
        array<str> nestedness;
        array<entity> & global;
        array<entity> & outer;
        array<entity> & body;

        void proceed (const array<statement> & input)
        {
            for (auto & s : input)
            {
                std::visit(aux::overloaded
                {
                    [&](loop_for    s) { add(s); },
                    [&](loop_while  s) { add(s); },
                    [&](expression  s) { add(s); },
                    [&](conditional s) { add(s); },
                    [&](declaration s) { add(s); },
                    [&](subroutine  s) { add(s); },
                    [&](pragma      s) { add(s); },
                },
                s.variant);
            }
        }

        void add (const loop_for    & s) {}
        void add (const loop_while  & s) {}
        void add (const expression  & s)
        {
        }
        void add (const conditional & s) {}
        void add (const declaration & s)
        {
            if (s.kind == "singleton") {
                str name = s.names[0]->text;
                body += entity{}; body.back().head += token{name + " " + name};
                outer += entity{}; outer.back().head += token{"struct " + name};
                context{nestedness + name, global, outer, outer.back().body}
                    .proceed(s.body);
            }
        }
        void add (const subroutine  & s)
        {

        }
        void add (const pragma      & s) {}
    };

    array<entity> proceed (const syntax::analysis::data & data)
    {
        array<entity> output;

        entity main; main.head += token{"int main (int argc_, char *argv_[])"};

        context global { array<str>{}, output, output, output};
        context mainer { array<str>{}, output, output, main.body};
        
        for (auto && s : data.statements)
        {
            std::visit(aux::overloaded
            {
                [&](loop_for    s) { mainer.add(s); },
                [&](loop_while  s) { mainer.add(s); },
                [&](expression  s) { mainer.add(s); },
                [&](conditional s) { mainer.add(s); },
                [&](declaration s) { mainer.add(s); },
                [&](subroutine  s) { global.add(s); },
                [&](pragma      s) { global.add(s); },
            },
            s.variant);
        }

        if (main.body.empty()) main.body += entity{};

        output += entity{}; output.back().head += token{"#include <iostream>"};
        main.body += entity{}; main.body.back().head += token{"std::cout << \"press any key...\""};
        main.body += entity{}; main.body.back().head += token{"std::cout << std::endl"};
        main.body += entity{}; main.body.back().head += token{"std::cin.get()"};

        output += main;

        return output;
    }
}

