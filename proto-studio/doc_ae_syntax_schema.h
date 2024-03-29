﻿#pragma once
#include <set>
#include "doc_ae_syntax_reader.h"
namespace doc::ae::syntax
{
    struct schema : reader
    {
        schema (array<element>&& input,
        report& log, std::atomic<bool>& cancel):
        reader(std::move(input), log, cancel) {}

        auto output () { return read_statements(); }

        auto read_statements () -> array<statement>
        {
            array<statement> ss; statement s;
            try
            {
                while (not input.empty())
                {
                    if (cancel) break;
                    if (next() == ";") 
                    read(";"); else {
                    read_statement(s);
                    ss += std::move(s); }
                }
            }
            catch (str const&) { log.error(
            gray("source: ") + dark(html::encoded(s.source)) + "<br" +
            gray("scheme: ") + blue(html::encoded(s.schema)) + "<br" +
            gray("names: " ) + dark(html::encoded(print(s.names  ))) + "<br" +
            gray("args: "  ) + dark(html::encoded(print(s.args   ))) + "<br" +
            gray("type: "  ) + dark(html::encoded(print(s.typexpr))) + "<br" +
            gray("kind: "  ) + dark(html::encoded(s.kind))); }
            return ss;
        }

        auto read_body () -> array<statement>
        {
            if (next() == ";") return array<statement>{};
            if (next() != "{") expected("body");

            auto statements = schema{std::move(
            input.front().elements), log, cancel}.output();
            input.pop_front();

            if (statements.empty())
                statements +=
                statement{.kind = "empty"};
            return statements;
        }

        auto read_statement_or_body () -> array<statement>
        {
            if (next() == "{") return read_body();
            array<statement> ss;
            ss += statement{}; read_statement(ss.back()); while (next() == ",") { read(",");
            ss += statement{}; read_statement(ss.back()); }
            return ss;
        }

        auto read_expression_or_body () -> array<statement>
        {
            array<statement> ss;
            if (next() == "{") return read_body();
            if (next() == ";") { read(); return ss; }
            if (next() != "=") expected("= and expression or body"); read("=");
            ss += statement{};
            ss.back().kind = "return";
            ss.back().expr = read_expression();
            return ss;
        }

        void read_statement (statement& s)
        {
            str& schema = s.schema;
            str& source = s.source;
            static token o {"(", "symbol"};
            static token c {")", "symbol"};
            s.args.opening = &o;
            s.args.closing = &c;

            auto replace_end = [&schema](str what, str with) {
            if (schema.ends_with(str(" " + what)) or schema == what) {
                schema.resize(schema.size() - what.size());
                schema += with; } };

            for (auto& e: input)
            {
                schema += " ";
                source += " " + e.print();

                if (e.opening->text == ";" ) break;
                if (e.opening->text == "{" ) break;
                if (e.opening->text == "(" ) schema += "()"; else
                if (e.opening->text == "[" ) schema += "[]"; else
                if (e.opening->text == "," ) schema += ",";  else
                if (e.opening->text == "::") schema += "::"; else
                if (e.opening->text == ":=") schema += ":="; else
                if (e.opening->text == str(u8"←")) schema += "<-"; else
                if (e.opening->text == str(u8"→")) schema += "->"; else
                if (e.opening->kind == "keyword")
                    schema += e.opening->text; else
                    schema += e.opening->kind;

                if (e.opening->kind == "keyword")
                if (e.opening->text == "import"
                or  e.opening->text == "goto"
                or  e.opening->text == "label"
                or  e.opening->text == "yield"
                or  e.opening->text == "return")
                    break;

                if (schema.starts_with(" operator "))
                    continue;

                if (schema.ends_with(" ="))
                    break;

                replace_end("name , name",     "names");
                replace_end("names , name",    "names");
                replace_end(":: name",         "namepack");
                replace_end("name ()",         "namepack");
                replace_end("name ::",         "namepack::");
                replace_end("namepack ()",     "namepack");
                replace_end("namepack ::",     "namepack::");
                replace_end("namepack:: name", "namepack");
            }

            schema.strip();
            auto schema_starts_with = [&schema](str s) { return
                 schema.starts_with(s) and (
                 schema.size() == s.size() or
                 schema[s.size()] == ' '); };

            if (schema == "")
            {
                if (next() == "{") s.body = read_body(); else
                if (next() != ";") unexpected(); else {
                s.kind = "empty"; read(";"); }
                return;
            }
            if (schema == "import")
            {
                s.kind = read("import")->text;
                //s.names = read_list_of_namepacks();
                return;
            }
            if (schema == "name")
            {
                s.kind = "singleton";
                s.names += read_name();
                s.body += read_body();
                return;
            }
            if (schema == "name :=")
            {
                s.kind = "variable";
                s.names += read_name(); read("=");
                s.expr = read_expression();
                return;
            }
            if (schema == "name name =" or
                schema == "name names =" or
                schema == "namepack name =" or
                schema == "namepack names =")
            {
                s.kind = "variable";
                s.typexpr = read_namepack();
                s.names = read_list_of_names(); read("=");
                s.expr = read_expression();
                return;
            }
            if (schema == "name name" or
                schema == "name names" or
                schema == "namepack name" or
                schema == "namepack names")
            {
                s.kind = "variable";
                s.typexpr = read_namepack();
                s.names = read_list_of_names();
                return;
            }
            if (schema_starts_with("let"))
            {
                s.kind = "alias"; read("let");
                s.names += read_name(); read("=");
                s.expr = read_expression();
                return;
            }
            if (schema_starts_with("type"))
            {
                s.kind = read()->text;
                s.names += read_name();
                s.names.back()->kind = "type";
                s.args = read_optional_args();

                if (next() == "extension")
                s.variety = read()->text;
                else
                if (next() == "extends"
                or  next() == "narrows"
                or  next() == "implements") {
                s.variety = read()->text;
                s.typexpr = read_namepack(); }

                s.body = read_expression_or_body();
                return;
            }
            if (schema_starts_with("property"))
            {
                s.kind = read("property")->text;
                s.names += read_name();
                if (next() == str(u8"→")) s.variety = "getter"; else
                if (next() == str(u8"←")) s.variety = "setter"; else
                expected(u8"→ or ←"); read();
                s.typexpr = read_namepack();
                s.body = read_expression_or_body();
                return;
            }
            if (schema_starts_with("function ()")
            or  schema_starts_with("function []"))
            {
                s.kind = read()->text;
                s.args = read_optional_args();
                s.typexpr = read_optional_type();
                s.body = read_expression_or_body();
                return;
            }
            if (schema_starts_with("function"))
            {
                s.kind = read()->text;
                s.names += read_name();
                s.args = read_optional_args();
                s.typexpr = read_optional_type();
                s.body = read_expression_or_body();
                return;
            }
            if (schema_starts_with("coercion"))
            {
                s.kind = read()->text;
                s.args.list += read_one_parameter(); read(u8"→");
                s.typexpr = read_namepack();
                s.body = read_expression_or_body();
                return;
            }
            if (schema_starts_with("operator"))
            {
                replace_end("name"  , "x");
                replace_end("symbol", "x");
                schema.replace_all(" name "  , " x ");
                schema.replace_all(" symbol ", " x ");
                s.kind = read("operator")->text;

                if (schema_starts_with("operator () x () x ()"))
                {
                    s.variety = "ternary";
                    s.args.list += read_one_parameter();
                    s.names += read_name_or_symbol();
                    s.names.back()->kind = "operator";
                    s.args.list += read_one_parameter();
                    s.names += read_name_or_symbol();
                    s.names.back()->kind = "operator";
                    s.args.list += read_one_parameter();
                }
                else
                if (schema_starts_with("operator () x ()")
                or  schema_starts_with("operator () ="))
                {
                    s.variety = "binary";
                    s.args.list += read_one_parameter();
                    s.names += read_name_or_symbol();
                    s.names.back()->kind = "operator";
                    s.args.list += read_one_parameter();
                }
                else
                if (schema_starts_with("operator () x"))
                {
                    s.variety = "postfix";
                    s.args.list += read_one_parameter();
                    s.names += read_name_or_symbol();
                    s.names.back()->kind = "operator";
                }
                else
                if (schema_starts_with("operator x ()")
                or  schema_starts_with("operator ="))
                {
                    s.variety = "prefix";
                    s.names += read_name_or_symbol();
                    s.names.back()->kind = "operator";
                    s.args.list += read_one_parameter();
                }
                else
                if (schema_starts_with("operator x"))
                {
                    s.variety = "self";
                    s.names += read_name_or_symbol();
                    s.names.back()->kind = "operator";
                }
                else
                expected("operator name or parameter");
                s.typexpr = read_optional_type();
                s.body = read_expression_or_body();
                return;
            }
            if (schema_starts_with("precedence"))
            {
                s.kind = read()->text;
                while (not input.empty() and next() != ";") {
                s.names += read_name_or_symbol();
                s.names.back()->kind = "operator"; }
                return;
            }

            if (schema_starts_with("for"))
            {
                s.kind = "for"; read("for");
                s.names = read_list_of_names(); read("in");
                s.expr = read_expression_until("do");
                s.body = read_statement_or_body();
                return;
            }
            if (schema_starts_with("while"))
            {
                s.kind = "while"; read("while");
                s.expr = read_expression_until("do");
                s.body = read_statement_or_body();
                return;
            }
            if (schema_starts_with("if"))
            {
                s.kind = "if"; read("if");
                s.expr = read_expression_until("then");
                s.body = read_statement_or_body();
                return;
            }
            if (schema_starts_with("else if"))
            {
                s.kind = "else if"; read("else"); read("if");
                s.expr = read_expression_until("then");
                s.body = read_statement_or_body();
                return;
            }
            if (schema_starts_with("else"))
            {
                s.kind = "else"; read("else");
                s.body = read_statement_or_body();
                return;
            }

            if (schema_starts_with("goto")
            or  schema_starts_with("label"))
            {
                s.kind = read()->text;
                s.names += read_name();
                if (next() != "") {
                if (next() == ";") read(";");
                else unexpected(); }
                return;
            }
            if (schema_starts_with("yield")
            or  schema_starts_with("return")
            or  schema_starts_with("assert")
            or  schema_starts_with("invariant"))
            {
                s.kind = read()->text;
                s.expr = read_expression();
                return;
            }

            s.kind = "expression";
            s.expr = read_expression();
        }
    };
}

