#pragma once
#include <set>
#include "doc_ae_syntax_reader.h"
namespace doc::ae::syntax
{
    const std::set<str> keywords = 
    {
        "using",
            
        "if", "then", "else",

        "for", "each", "in", "while", "until", "do",

        "await", "yield", "return", "break", "continue", 

        "operator", "type", "extension",
        "function",
        "mutation",
    };

    struct schema : reader
    {
        array<statement> output;
        array<cluster> && input; report& log; schema(
        array<cluster> && input, report& log, std::atomic<bool>& stop) :
        reader(array<element>{}, log, stop),
        input(std::move(input)), log(log)
        {
            statement s;

            // if (not log.errors.empty()) return;

            for (auto && cluster : input) try
            {
                if (stop) break;

                reader::input = deque{std::move(cluster.elements)};

                s = read_statement();

                if (s.kind == "")
                {
                    reader::input += read_clusters(std::move(cluster.clusters));
                    if (not reader::input.empty()) s.expr = read_expression();
                    output += std::move(s);
                    continue;
                }

                if((s.kind == "type" or
                    s.kind == "function" or
                    s.kind == "function" or
                    s.kind.contains("operator"))
                    and reader::next() == "=")
                {
                    reader::input += read_clusters(std::move(cluster.clusters));
                    read("="); s.expr = read_expression();
                    output += std::move(s);
                    continue;
                }

                if (not reader::input.empty())
                s.body += read_statement();
                s.body += schema(std::move(cluster.clusters), log, stop).output;
                output += std::move(s);
            }
            catch (const str & what) {
                str info = "<font color=#000080>";
                info += "  kind: \"<font color=#4000B0>" + s.kind + "\"</font>"; 
                info += ", scheme: <font color=#800080>" + s.schema + "</font>"; 
                info += ", source: <font color=#808080>" + s.source + "</font>"; 
                log.error(info);
            }
        }

        deque<element> read_clusters (array<cluster> && clusters)
        {
            deque<element> elements; for (auto && cluster : clusters)
            {
                elements += deque<element>(std::move(cluster.elements));
                elements += read_clusters (std::move(cluster.clusters));
            }
            return elements;
        }

        statement read_statement ()
        {
            statement s;
            static token o {"(", "symbol"};
            static token c {")", "symbol"};
            s.args.opening = &o;
            s.args.closing = &c;

            str & schema = s.schema;
            str & source = s.source;

            for (auto & e : reader::input)
            {
                schema += " ";
                source += " " + print(e);

                if (e.opening and
                    e.opening->kind == "name" and
                    keywords.contains(e.opening->text))
                    e.opening->kind = "keyword";

                if (e.kind == "()") schema += "()"; else
                if (e.opening->text == "=" ) schema += "=";  else
                if (e.opening->text == "," ) schema += ",";  else
                if (e.opening->text == ":" ) schema += ":";  else
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
                        schema += with;
                    }
                };

                replace("name , name", "names");
                replace("names , name", "names");
            
                replace(":: name", "namepack");
                replace("name ()", "namepack");
                replace("name ::", "namepack::");
                replace("namepack ()", "namepack");
                replace("namepack ::", "namepack::");
                replace("namepack:: name", "namepack");
            }
            schema.upto(1).erase(); // leading " "
            source.upto(1).erase(); // leading " "

            auto schema_starts_with = [&schema](str s) { return
                 schema.starts_with(s) and (
                 schema.size() == s.size() or
                 schema[s.size()] == ' ');
            };

            if (schema == "name")
            {
                s.kind = "singleton";
                s.name = read_name();
                return s;
            }
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
            if (schema_starts_with("mutation name <-"))
            {
                s.kind = read()->text;
                s.name = read_name(); read((char*)(u8"←"));
                s.type = read_namepack();
                s.args.list += parameter{read_name()};
                return s;
            }
            if (schema_starts_with("function")
            or  schema_starts_with("mutation"))
            {
                s.kind = read()->text;
                s.name = read_name();
                s.args = read_optional_args();
                s.type = read_optional_type();
                return s;
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
                    s.name = read_name();
                    s.args.list += read_one_parameter();
                }
                else if (schema_starts_with("operator () x"))
                {
                    s.kind = "postfix operator";
                    s.args.list += read_one_parameter();
                    s.name = read_name();
                }
                else if (schema_starts_with("operator x ()"))
                {
                    s.kind = "prefix operator";
                    s.name = read_name();
                    s.args.list += read_one_parameter();
                }
                else read("operator name or parameter"); // throw
                s.type = read_optional_type();
                return s;
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

            return s;
        }
    };
}

