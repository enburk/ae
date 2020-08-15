#pragma once
#include <span>
#include <iostream>
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::synthesis
{
    void out (std::span<uint8_t> span)
    {
        std::cout.write((char*)(span.data()), span.size());
        std::cout.flush();
    }


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
                body += entity{"type_" + name + " " + name};
                outer += entity{"struct type_" + name};
                outer.back().kind = "class";
                context{nestedness + name, global, outer,
                    outer.back().body}.proceed(s.body);
                if (outer.back().body.empty())
                    outer.back().body += entity{};
            }
        }
        void add (const subroutine & s)
        {
            auto & scope = body;

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

            if (&body != &global)
                head += "static ";

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

    void proceed (
        entity  & cpp,
        context & mainer,
        context & global,
        syntax::analysis::data & data)
    {
        if (data.synthesized) return; data.synthesized = true;

        for (auto dependency : data.dependencies)
            proceed(cpp, mainer, global,
                syntax::analysis::sources[dependency]);

        if (data.cpp)
        {
            token tt;
            for (const auto & t : data.tokens) tt.text += t.text;
            cpp.head += tt;
            return;
        }

        for (const auto & s : data.statements)
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
    }

    array<entity> proceed (syntax::analysis::data & data)
    {
        array<entity> output;
        output += entity{"#include <span>"};
        output += entity{"#include <cstdint>"};

        entity main; main.head += token{"int main (int argc_, char *argv_[])"};
        entity cpp;

        context global { array<str>{}, output, output, output};
        context mainer { array<str>{}, output, output, main.body};

        for (auto dependency : data.dependencies)
            syntax::analysis::sources[dependency].synthesized = false;

        proceed(cpp, mainer, global, data);

        if (main.body.empty()) main.body += entity{};

        output += entity{"#include <iostream>"};

        main.body += entity{"std::cout << \"\\n\""};
        main.body += entity{"std::cout << \"press Enter to close...\""};
        main.body += entity{"std::cout << std::endl"};
        main.body += entity{"std::cin.get()"};

        output += main;
        output += cpp;

        return output;
    }
}

