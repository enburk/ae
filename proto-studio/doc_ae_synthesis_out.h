#pragma once
#include "doc_text_text.h"
#include "doc_ae_synthesis_out_aux.h"
#include "doc_ae_synthesis_out_out.h"
namespace doc::ae::synthesis
{
    void out (statement const& st, array<entity>& body)
    {
        str kind = st.kind;
        str name = print(st.names);
        str type = print(st.typexpr, true);
        str args = print(st.args);
        str expr = print(st.expr);

        str template_;
        for (auto& arg: st.args.list) {
            str name = print(arg.name);
            str type = print(arg.typexpr, true);
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
        body += entity{.info = "kind: "   + st.kind};

        entity e;
        for (auto& statement: st.body)
            out(statement, e.body);

        if (kind == "empty")
        {
            e.kind = "empty";
        }
        if (kind == "singleton")
        {
            e.kind = "class";
            e.head = "struct type_" + name;
        }
        if (kind == "type")
        {
            e.kind = "class";
            e.head =  template_;
            e.head += "struct " + name;
        }
        if (kind == "function")
        {
            e.kind = "function";
            e.head =  template_;
            e.head += "auto " + name + args + " const ";
            if (type == "" and e.body.empty()) type = "void";
            if (type != "") e.head += " -> " + type;
        }
        if (kind == "mutation")
        {
            e.kind = "function";
            e.head =  template_;
            e.head += "auto " + name + args;
            if (type == "" and e.body.empty()) type = "void";
            if (type != "") e.head += " -> " + type;
        }
        if (kind == "yield")
        {
            e.kind = "directive";
            e.head =  "co_yield"; if (expr != "void")
            e.head += " " + expr;
        }
        if (kind == "return")
        {
            e.kind = "directive";
            e.head =  "return"; if (expr != "void")
            e.head += " " + expr;
        }
        if (kind == "expression")
        {
            e.kind = "expression";
            e.head = print(st.expr);
        }

        body += e;

        if (kind == "singleton")
        body += entity {"type_" +
            name + " " + name};
    }
}

