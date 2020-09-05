#pragma once
#include <span>
#include <iostream>
#include <filesystem>
#include "doc_ae_syntax_analysis.h"
#include "doc_ae_synthesis_print.h"
namespace doc::ae::synthesis
{
    using namespace doc::ae::syntax;

    struct context
    {
        array<str> nestedness;
        array<entity> & body;

        void proceed (const array<statement> & input)
        {
            for (auto & s : input)
            {
                std::visit(aux::overloaded
                {
                    [&](noop        s) { add(s); },
                    [&](loop_for    s) { add(s); },
                    [&](loop_while  s) { add(s); },
                    [&](expression  s) { add(s); },
                    [&](conditional s) { add(s); },
                    [&](declaration s) { add(s); },
                    [&](definition  s) { add(s); },
                    [&](subroutine  s) { add(s); },
                    [&](pragma      s) { add(s); },
                },
                s.variant);
            }
        }

        void add (const noop        & s) { body += entity{}; }
        void add (const loop_for    & s) {}
        void add (const loop_while  & s) {}
        void add (const expression  & s)
        {
            body += entity{print(s)}; 
        }
        void add (const conditional & s) {}
        void add (const declaration & s)
        {
            if (s.kind == "variable"
            or  s.kind == "constant")
            {
                entity e;
                str prefix = s.kind == "constant" ? "constexpr " : "";
                e.head += token{prefix + print_type(s.type)};
                for (auto & name : s.names) {
                    e.head += token{print(name->text)};
                    e.head += token{","};
                }
                e.head.truncate();
                if (not s.body.empty()) {
                    e.head += token{"="};
                    context{nestedness, e.body}
                    .proceed(s.body);
                }
                body += e;
            }
        }
        void add (const definition & s)
        {
            if (s.kind == "singleton")
            {
                str name = print(s.name->text);
                body += entity{"struct type_"+name};
                body.back().kind = "class";
                context{nestedness + name, body.back().body}.proceed(s.body);
                body += entity{"type_"+name + " " + name};
            }
            if (s.kind == "type")
            {
                str templ;
                for (auto & p : s.parameters) {
                    str type = print(p.type);
                    str name = print(p.name->text);
                    if (type == "type" or type == "") type = "typename";
                    templ += type + " " + name + ", ";
                }
                templ.truncate(); // ","
                templ.truncate(); // " "
                if (templ != "") templ = "template<" + templ + "> ";

                str name = print(s.name->text);
                body += entity{templ + "struct " + name};
                body.back().kind = "class";
                context{nestedness + name, body.back().body}.proceed(s.body);
            }
        }
        void add (const subroutine & s)
        {
            str head, args;
            for (auto & p : s.parameters.list) {
                str type = print(p.type);
                str name = print(p.name->text);
                if (type == "") {
                    head += "class type_"+name + ", ";
                    args += "type_"+name + " " + name + ", "; }
                else args += print_type(p.type) + " " + name + ", ";
            }
            head.truncate(); args.truncate(); // ","
            head.truncate(); args.truncate(); // " "
            if (head != "") head = "template<" + head + "> ";

            str type = print_type(s.type);
            str name = print(s.name->text);
            body += entity{head + type + " " + name + "(" + args + ")"};
            body.back().kind = "function";
            context{nestedness + name, body.back().body}.proceed(s.body);
        }
        void add (const pragma & s) {}
    };

    void proceed (syntax::analysis::data & data,
        context & global,
        context & main,
        entity  & cpp)
    {
        if (data.synthesized) return; data.synthesized = true;

        for (auto dependency : data.dependencies)
            proceed(syntax::analysis::sources[dependency],
                global, main, cpp);

        if (data.cpp) {
            cpp.head += token{"#include \"" +
                data.source.string() + "\""};
            return;
        }

        str module_name = data.source.stem().string();
        global.body += entity{"namespace " + module_name};
        auto & body = global.body.back().body;
        context context { array<str>{}, body};

        for (const auto & s : data.statements)
        {
            std::visit(aux::overloaded
            {
                [&](noop        s) { main.add(s); },
                [&](loop_for    s) { main.add(s); },
                [&](loop_while  s) { main.add(s); },
                [&](expression  s) { main.add(s); },
                [&](conditional s) { main.add(s); },
                [&](declaration s) { context.add(s); },
                [&](definition  s) { context.add(s); },
                [&](subroutine  s) { context.add(s); },
                [&](pragma      s) { context.add(s); },
            },
            s.variant);
        }

        if (body.empty()) global.body.truncate();
    }

    array<entity> proceed (syntax::analysis::data & data)
    {
        for (auto dependency : data.dependencies)
            syntax::analysis::sources[dependency]
            .synthesized = false;

        array<entity> output;
        output += entity{"#include <span>"};
        output += entity{"#include <cstdint>"};
        output += entity{"namespace ae"};
        auto & ae = output.back().body;

        entity cpp;
        entity main; main.head += token{"void main_()"};

        context global { array<str>{}, ae};
        context mainer { array<str>{}, main.body};

        proceed(data, global, mainer, cpp);

        if (main.body.empty())
            main.body +=
            entity{};

        ae += main; output += cpp;

        return output;
    }
}

