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

    struct schema
    {
        array<statement> output;
        array<cluster> && input; report& log; schema(
        array<cluster> && input, report& log) :
            input(std::move(input)), log(log)
        {
            for (auto && cluster : input)
            {
                reader r(std::move(cluster.elements), log); try
                {
                    if (auto d = read_definition(r); d.name)
                    {
                        if (not r.input.empty())
                        d.body += statement{r.read_expression()};
                        d.body += schema(std::move(cluster.clusters), log).output;
                        output += statement{std::move(d)};
                        continue;
                    }

                    r.input += read_expression(std::move(cluster.clusters));
                    auto e = r.read_expression();
                    output += statement{std::move(e)};
                }
                catch (const std::exception & e) {
                    log.error(r.last_token,
                        e.what()); }
            }
        }

        deque<element> read_expression (array<cluster> clusters)
        {
            deque<element> elements;
            for (auto && cluster : clusters)
            {
                elements += deque<element> (std::move(cluster.elements));
                elements += read_expression(std::move(cluster.clusters));
            }
            return elements;
        }

        definition read_definition (reader & r)
        {
            definition d;
            str & schema = d.schema;
            str & source = d.source;

            for (auto & e : r.input)
            {
                schema += " ";
                source += " " + (e.kind == "()" ? e.kind : e.opening->text);

                if (e.opening and
                    e.opening->kind == "name" and keywords.contains(e.opening->text))
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
            
                if (schema.ends_with(" names , name")) schema.resize(schema.size()-7);
                if (schema.ends_with(" name , name")){ schema.resize(schema.size()-7);
                    schema += "s"; }
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
                d.kind = "singleton";
                d.name = r.read_name();
                return d;
            }
            if (schema_starts_with("mutation name <-"))
            {
                d.kind = r.read()->text;
                d.name = r.read_name(); read((char*)(u8"←"));
                d.type = r.read_namepack();
                static token o {"(", "symbol"};
                static token c {")", "symbol"};
                d.args.opening = &o;
                d.args.closing = &c;
                d.args.list += parameter{r.read_name()};
                r.read("=");
                return d;
            }
            if (schema_starts_with("function")
            or  schema_starts_with("mutation"))
            {
                d.kind = r.read()->text;
                d.name = r.read_name();
                d.args = r.read_optional_args();
                d.type = r.read_optional_type();
                r.read("=");
                return d;
            }

            return d;
        }
    };
}

