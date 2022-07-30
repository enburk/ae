#pragma once
#include <set>
#include "doc_ae_syntax_reader.h"
namespace doc::ae::syntax
{
    struct schema : reader
    {
        array<statement> output; report& log; std::atomic<bool>& cancel; schema(
        array<element>&& input,  report& log, std::atomic<bool>& cancel) :
        reader(std::move(input), log, cancel), log(log), cancel(cancel)
        {
            output = read_statements();
        }

        auto read_statements () -> array<statement>
        {
            array<statement> ss; statement s;
            try
            {
                while (not input.empty())
                {
                    if (cancel) break;
                    read_statement(s);
                    ss += std::move(s);
                }
            }
            catch (str const&) { log.error(
            gray("source: ") + dark(s.source) + "<br" +
            gray("scheme: ") + blue(s.schema) + "<br" +
            gray("names: " ) + dark(print(s.names  )) + "<br" +
            gray("args: "  ) + dark(print(s.args   )) + "<br" +
            gray("type: "  ) + dark(print(s.typexpr)) + "<br" +
            gray("kind: "  ) + dark(s.kind)); }
            return ss;
        }

        auto read_body () -> array<statement>
        {
            if (next() == ";") return array<statement>{};
            if (next() != "{") expected("body");

            auto statements = schema(std::move(
            input.front().elements), log, cancel).output;
            input.pop_front();

            if (statements.empty())
                statements +=
                statement{};
            return statements;
        }

        void read_statement (statement& s)
        {
            str& schema = s.schema;
            str& source = s.source;
            static token o {"(", "symbol"};
            static token c {")", "symbol"};
            s.args.opening = &o;
            s.args.closing = &c;

            for (auto& e: input)
            {
                schema += " ";
                source += " " + e.print();

                if (e.opening->text == "=" ) schema += "=";
                if (e.opening->text == "=" ) break;
                if (e.opening->text == ";" ) break;
                if (e.opening->text == "{" ) break;
                if (e.opening->text == "(" ) schema += "()"; else
                if (e.opening->text == "[" ) schema += "[]"; else
                if (e.opening->text == "," ) schema += ",";  else
                if (e.opening->text == "::") schema += "::"; else
                if (e.opening->text == ":=") schema += ":="; else
                if (e.opening->text == (char*)(u8"←")) schema += "<-"; else
                if (e.opening->text == (char*)(u8"→")) schema += "->"; else
                if (e.opening->kind == "keyword")
                    schema += e.opening->text; else
                    schema += e.opening->kind;

                if (schema.starts_with(" operator ")) continue;

                auto replace = [&schema](str what, str with) {
                if (schema.ends_with(str(" " + what)) or schema == what) {
                    schema.resize(schema.size() - what.size());
                    schema += with; } };

                replace("name , name",     "names");
                replace("names , name",    "names");
                replace(":: name",         "namepack");
                replace("name ()",         "namepack");
                replace("name ::",         "namepack::");
                replace("namepack ()",     "namepack");
                replace("namepack ::",     "namepack::");
                replace("namepack:: name", "namepack");
            }
            schema.upto(1).erase(); // leading " "
            source.upto(1).erase(); // leading " "

            auto schema_starts_with = [&schema](str s) { return
                 schema.starts_with(s) and (
                 schema.size() == s.size() or
                 schema[s.size()] == ' '); };

            if (schema == "")
            {
                if (next() == "{") s.body = read_body(); else
                if (next() != ";") unexpected(); else
                read(";");
                return;
            }
            if (schema == "name")
            {
                s.kind = "singleton";
                s.names += read_name();
                s.body += read_body();
                return;
            }
            if (schema_starts_with("type"))
            {
                s.kind = read()->text;
                s.names += read_name();
                s.args = read_optional_args();
                if (next() == "extends"
                or  next() == "subset.of") {
                s.kind += " " + read()->text;
                s.typexpr = read_namepack(); }
                if (next() == "=") { read("=");
                s.expr = read_expression(); } else
                s.body = read_body();
                return;
            }
            if (schema_starts_with("function")
            or  schema_starts_with("mutation"))
            {
                s.kind = read()->text;
                s.names += read_name();
                s.args = read_optional_args();
                s.typexpr = read_optional_type();
                if (next() == "=") { read("=");
                s.expr = read_expression(); } else
                s.body = read_body();
                return;
            }
            if (schema_starts_with("operator"))
            {
                schema.replace_all(" name "  , " x ");
                schema.replace_all(" symbol ", " x ");
                read("operator");

                if (schema_starts_with("operator () x ()"))
                {
                    s.kind = "binary operator";
                    s.args.list += read_one_parameter();
                    s.names += read_name();
                    s.args.list += read_one_parameter();
                }
                else if (schema_starts_with("operator () x"))
                {
                    s.kind = "postfix operator";
                    s.args.list += read_one_parameter();
                    s.names += read_name();
                }
                else if (schema_starts_with("operator x ()"))
                {
                    s.kind = "prefix operator";
                    s.names += read_name();
                    s.args.list += read_one_parameter();
                }
                else read("operator name or parameter"); // throw
                s.typexpr = read_optional_type();
                if (next() == "=") { read("=");
                s.expr = read_expression(); } else
                s.body = read_body();
                return;
            }

            //if (schema_starts_with("for"))
            //{
            //    read("for"); auto names = read_list_of_names();
            //    read("in");  auto range = read_expression_until("do");
            //    auto in = unique_id("in");
            //    auto it = unique_id("it");
            //    s.name = in; s.kind = "variable"; s.expr = ":=" + range;
            //    output += std::move(s); s = statement{};
            //    s.name = it; s.kind = "variable"; s.expr = token{":="} + it + "::" + "begin";
            //    output += std::move(s); s = statement{};
            //    s.kind = "while";
            //    s.expr = it + u8"≠" + range + "::" + "end";
            //    for (int i=1; auto name : names) {
            //        statement ss;
            //        ss.name = name;
            //        ss.kind = "variable";
            //        ss.expr = token{":="} + range + "::" + "value" + "::" + str(i++);
            //        s.body += std::move(ss);
            //    }
            //}
            //if (schema == "name name" or
            //    schema == "name names" or
            //    schema == "namepack name" or
            //    schema == "namepack names")
            //{
            //    s.kind = "variable";
            //    s.name = read_name();
            //    return s;
            //}
            //if (schema == "name : name")
            //{
            //    s.kind = "singleton";
            //    s.name = read_name();
            //    return s;
            //}
            //if (schema_starts_with("name name ="))
            //{
            //    s.kind = read()->text;
            //    s.name = read_name(); read((char*)(u8"←"));
            //    s.type = read_namepack();
            //    s.args.list += parameter{read_name()};
            //    read("=");
            //    return s;
            //}

            s.kind = "expression";
            s.expr = read_expression();
        }
    };
}

