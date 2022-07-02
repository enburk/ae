#pragma once
#include "doc_ae_transtr.h"
#include "doc_ae_synthesis.h"
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::translator
{
    struct context
    {
        array<entity> & body;

        void proceed (const array<statement> & input)
        {
            for (auto& st: input)
            {
                str kind = st.kind;
                str name = print(st.names);
                str type = print(st.typexpr);
                str args = print(st.args);

                str template_;
                for (auto & arg : st.args.list) {
                    str name = print(arg.name->text);
                    str type = print(arg.typexpr);
                    if (type == "") template_ +=
                        "typename type_" +
                            name + ", ";
                }
                template_.truncate();
                template_.truncate();
                if (template_ != "")
                    template_ = "template<" + template_ + "> ";

                body += entity{.info = "source: " + st.source};
                body += entity{.info = "schema: " + st.schema};

                entity e;
                if (not st.body.empty())
                    context{e.body}.proceed(st.body);

                if (kind == "singleton")
                {
                    e.kind = "class";
                    e.head = "struct type_" + name; body += e;
                    body += entity {"type_" + name + " " + name};
                    continue;
                }
                if (kind == "function")
                {
                    e.kind = "function";
                    e.head =  template_;
                    e.head += "auto " + name + args + " const ";
                    if (type != "") e.head += " -> " + type;
                    body += e;
                    continue;
                }
                if (kind == "mutation")
                {
                    e.kind = "function";
                    e.head =  template_;
                    e.head += "auto " + name + args;
                    if (type != "") e.head += " -> " + type;
                    body += e;
                    continue;
                }

                if (kind != "") body += entity{.info = "kind: " + kind};
                if (name != "") body += entity{.info = "name: " + name};
                if (type != "") body += entity{.info = "type: " + type};
                if (args != "") body += entity{.info = "args: " + args};
                e.head = print(st.expr);
                body += e;
            }
        }
    };

    // void proceed
    // (
    //     syntax::analysis::data & data,
    //     context & global,
    //     context & main,
    //     entity  & cpp)
    // {
    //     str module_name = data.source.stem().string();
    //     global.body += entity{"namespace " + module_name};
    //     auto & body = global.body.back().body;
    //     context context { array<str>{}, body};
    // 
    //     for (const auto & st : data.statements)
    //     {
    //         if (kind == "singleton")
    // 
    // 
    //         // std::visit(aux::overloaded
    //         // {
    //         //     [&](noop        s) { main.add(s, st.scope); },
    //         //     [&](loop_for    s) { main.add(s, st.scope); },
    //         //     [&](loop_while  s) { main.add(s, st.scope); },
    //         //     [&](expression  s) { main.add(s, st.scope); },
    //         //     [&](conditional s) { main.add(s, st.scope); },
    //         //     [&](directive   s) { main.add(s, st.scope); },
    //         //     [&](declaration s) { context.add(s, st.scope); },
    //         //     [&](definition  s) { context.add(s, st.scope); },
    //         //     [&](subroutine  s) { context.add(s, st.scope); },
    //         //     [&](pragma      s) { context.add(s, st.scope); },
    //         // },
    //         // st.variant);
    //     }
    // 
    //     if (body.empty()) global.body.truncate();
    // }

    array<str> proceed (syntax::analysis::data& data)
    {
        array<entity> output;
        output += entity{"#include <span>"};
        output += entity{"#include <cstdint>"};
        output += entity{""};
        output += entity{"namespace ae"};
        auto & ae = output.back().body;

        entity cpp;
        entity main; main.head = "void main_()";

        // context global {ae};
        // context mainal {main.body};

        //proceed(data, global, mainer, cpp);

        if (main.body.empty())
            main.body +=
            entity{};

        ae += main; output += cpp;

        array<str> lines;
        for (auto && entity : output)
            entity.print(lines);

        return lines;
    }
}

