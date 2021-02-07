#pragma once
#include "doc_ae_syntax_reader.h"
namespace doc::ae::syntax
{
    array<statement> schema (array<cluster> input)
    {
        for (auto && cluster : input)
        {
            reader read(std::move(cluster.elements), log);

            try
            {
                elements = deque(std::move(element.elements));

                statement s = read_statement();

                output += std::move(s);
            }
            catch (const std::exception & e) { log.error(last_token, e.what()); }
        }
    }

    struct scheme
    {
        array<statement> output;
        array<element> && input; report& log; scheme(
        array<element> && input, report& log) :
            input(std::move(input)), log(log)
        {
            for (auto && cluster : input)
            {
                reader read(std::move(cluster.elements), log);
            }

            for (auto && element : input)
            {
                try
                {
                    elements = deque(std::move(element.elements));

                    statement s = read_statement();

                    if (aux::got<conditional>(s.variant) and
                        std::get<conditional>(s.variant).title->text == "else")
                    {
                        if (output.empty()
                        or not aux::got<conditional>(output.back().variant)
                        or not std::get<conditional>(output.back().variant).else_body.empty())
                            throw error("'else' without 'if'");

                        std::get<conditional>(output.back().variant).else_body = std::move(
                        std::get<conditional>(s.variant).else_body);

                        continue;
                    }

                    // if (log.errors.size() > 0) break;

                    output += std::move(s);
                }
                catch (const std::exception & e) { log.error(last_token, e.what()); }
            }
        }

        statement read_statement ();
    };

    inline statement scheme::read_statement ()
    {
        str schema;
        str source;

        for (auto & e : elements)
        {
            schema += " ";
            source += " " + (e.kind == "()" ? e.name : e.opening->text);

            keywording(e);

            if (e.name == "{}") schema += "{}"; else
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
            if (schema.ends_with(" name , name")){ schema.resize(schema.size()-7); schema += "s"; }
        }
        schema.upto(1).erase(); // leading " "
        source.upto(1).erase(); // leading " "
    }
}

