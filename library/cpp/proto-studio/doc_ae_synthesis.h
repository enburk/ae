#pragma once
#include <span>
#include <iostream>
#include <filesystem>
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

        static str encoded (brackets b)
        {
            array<entity> ee; context
            {.global=ee, .outer=ee, .body=ee}.proceed(b.list);

            str es;
            for (entity & e : ee) {
                for (auto && t : e.head)
                    es += t.text + " ";
                es.truncate();
                es += ",";
                es += " ";
            }
            es.truncate();
            es.truncate();

            str s;
            if (b.opening) s += b.opening->text; s += es;
            if (b.closing) s += b.closing->text;
            return s;
        }
        static str encoded (str name)
        {
            if (name.contains("_")) return name;
            name = html::lexica::encoded(name);
            name.replace_all("&", "_");
            name.replace_all(";", "_");
            if (name.contains("_")) name = "name_" + name;
            name.replace_all("__", "_");
            return name;
        }
        static str encoded (named_unit name)
        {
            str s;
            if (name.coloncolon) s += name.coloncolon->text;
            if (name.identifier) s += encoded(name.identifier->text);
            for (auto & p : name.parameters) s += encoded(p);
            return s;
        }
        static str encoded (named_pack name)
        {
            str s; for (auto & unit : name.units) s += encoded(unit);
            return s;
        }
        static str encoded_type (named_pack name)
        {
            bool range = false;
            str type = encoded(name);
            if (type == "") return "auto";
            if (type.ends_with("[]")) {
                type.truncate();
                type.truncate();
                range = true;
            }
            if (type == "byte") type = "uint8_t";
            if (range) type = "std::span<" + type + ">";
            return type;
        }

        array<token> encoded (expression e)
        {
            array<token> tokens;

            std::visit(aux::overloaded
            {
                [&](number  v) { tokens += token{v.token->text}; },
                [&](literal v) { tokens += token{str()

                    + "std::span<uint8_t>((uint8_t*)(" + v.token->text + "), "
                        + std::to_string(v.token->text.size() - 2) + ")"};
                },
                [&](named_pack v) { tokens += token{encoded(v)}; },
                [&](operation  v) {

                    if (v.title) tokens += token{encoded(v.title->text)};
                    
                    array<token> tt;
                    for (auto o : v.operands)
                    tt += encoded(o) + token{","};
                    tt.truncate();

                    tokens += token{"("} + tt + token{")"};
                },
                [&](brackets v)
                {
                    tokens += token{encoded(v)};
                },
                [&](expression_if  v) {},
                [&](expression_for v) {},
            },
            e.variant);

            return tokens;
        }

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
            body += entity{}; 
            body.back().head = encoded(s);
        }
        void add (const conditional & s) {}
        void add (const declaration & s)
        {
            if (s.kind == "singleton") {
                str name = encoded(s.names[0]->text);

                outer += entity{"namespace " + name};
                context{nestedness + name, global, outer,
                    outer.back().body}.proceed(s.body);
                if (outer.back().body.empty())
                    outer.back().body += entity{};

            //  body += entity{"type_" + name + " " + name};
            //  outer += entity{"struct type_" + name};
            //  outer.back().kind = "class";
            //  context{nestedness + name, global, outer,
            //      outer.back().body}.proceed(s.body);
            //  if (outer.back().body.empty())
            //      outer.back().body += entity{};
            }
            if (s.kind == "type") {
                str name = encoded(s.names[0]->text);
                outer += entity{"struct " + name};
                outer.back().kind = "class";
                context{nestedness + name, global, outer,
                    outer.back().body}.proceed(s.body);
                if (outer.back().body.empty())
                    outer.back().body += entity{};
            }
            if (s.kind == "variable") {
                body += entity{};
                body.back().head += token{encoded_type(s.type)};
                for (auto & name : s.names) {
                    body.back().head += *name;
                    body.back().head += token{","};
                }
                body.back().head.truncate();
                if (not s.body.empty()) {
                    body.back().head += token{"="};
                    context{nestedness, global, outer,
                        body.back().body}.proceed(s.body);
                }
            }
        }
        void add (const subroutine & s)
        {
            auto & scope =
                s.kind.starts_with("operator ") or
                s.kind.ends_with(" operator") ?
                global : body;

            str head, args;
            for (auto & p : s.parameters) {
                str type = encoded(p.type);
                str name = encoded(p.name->text);
                if (type == "") {
                    head += "class type_" + name + ", ";
                    args += "type_" + name + " " + name + ", "; }
                else args += encoded_type(p.type) + " " + name + ", ";
            }
            head.truncate(); args.truncate(); // ","
            head.truncate(); args.truncate(); // " "
            if (head != "")
                head = "template<" + head + "> ";

            // if (&body != &global)
            //     head += "static ";

            str type = encoded_type(s.type);
            str name = encoded(s.name->text);
            scope += entity{head + type + " " + name + "(" + args + ")"};
            scope.back().kind = "function";
            context{nestedness + name, global, outer,
                scope.back().body}.proceed(s.body);
            if (scope.back().body.empty() && not s.external)
                scope.back().body += entity{};
            else
            {
            }

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
        // if (module_name == "") return;
        global.body += entity{"namespace " + module_name};
        auto & body = global.body.back().body;
        context context { array<str>{}, global.body, global.body, body};

        for (const auto & s : data.statements)
        {
            std::visit(aux::overloaded
            {
                [&](loop_for    s) { main.add(s); },
                [&](loop_while  s) { main.add(s); },
                [&](expression  s) { main.add(s); },
                [&](conditional s) { main.add(s); },
                [&](declaration s) { main.add(s); },
                [&](subroutine  s) { context.add(s); },
                [&](pragma      s) { context.add(s); },
            },
            s.variant);
        }

        if (body.empty()) body += entity{};
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

        context global { array<str>{}, ae, ae, ae};
        context mainer { array<str>{}, ae, ae, main.body};

        proceed(data, global, mainer, cpp);

        if (main.body.empty())
            main.body +=
            entity{};

        ae += main; output += cpp;

        return output;
    }
}

