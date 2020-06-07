#pragma once
#include <set>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    static const std::set<str> keywords = // in front of statements
    {
        "if", "else", "for", "do", "while", "until", // "switch"
            
        "goto", // bodyless

        "operator", "class",
        "function", "union",
        "mutation", "type",
        "variable",
        "constant",
    };

    void schema (statement & statement)
    {
        array<element> elements;
        array<name> names;
        array<str> ids; 

        str schema; for (auto & e : statement.elements)
        {
            elements += e; if (elements.size() == 5) break;

            if (std::holds_alternative<token*>(e)) {
                if (std::get<token*>(e)->kind == "literal") schema += "literal "; else
                if (std::get<token*>(e)->kind == "number" ) schema += "number "; else
                if (std::get<token*>(e)->kind == "symbol" ) schema +=
                    std::get<token*>(e)->text + " "; else error(
                    std::get<token*>(e), "unexpected token");
            }
            else
            if (std::holds_alternative<brackets>(e)) {
                schema += std::get<brackets>(e).opening->text;
                schema += std::get<brackets>(e).closing->text;
                schema += " ";
            }
            else
            if (std::holds_alternative<name>(e)) {
                names += std::get<name>(e);
           
                if (names.back().names.size() == 1) { str id =
                    names.back().names[0].identifier->text;
                    if (keywords.find(id) != keywords.end()) {
                        schema += id + " ";
                        names.back().names[0].identifier->kind = "keyword";
                    }
                    else {
                        schema += "id ";
                        ids += id;
                    }
                }
                else schema += "name ";
            }
        }

        if (statement.body.size() > 0) schema += "{} ";

        schema.truncate();

        if (schema == "id {}") { statement.kind = "singleton"; statement.id = ids[0]; } else

        if (schema == "operator id = {}") { statement.kind = "operator"; statement.id = ids[0]; } else
        if (schema == "function id = {}") { statement.kind = "function"; statement.id = ids[0]; } else
        if (schema == "mutation id = {}") { statement.kind = "mutation"; statement.id = ids[0]; } else
        if (schema == "variable id = {}") { statement.kind = "variable"; statement.id = ids[0]; } else
        if (schema == "constant id = {}") { statement.kind = "constant"; statement.id = ids[0]; } else

        if (schema.starts_with("if "   )) { statement.kind = "block"; statement.id = "if"; } else
        if (schema.starts_with("else " )) { statement.kind = "block"; statement.id = "else"; } else
        if (schema.starts_with("for "  )) { statement.kind = "block"; statement.id = "for"; } else
        if (schema.starts_with("do "   )) { statement.kind = "block"; statement.id = "do"; } else
        if (schema.starts_with("while ")) { statement.kind = "block"; statement.id = "while"; } else
        if (schema.starts_with("until ")) { statement.kind = "block"; statement.id = "until"; } else

        if (schema.starts_with("goto ")) { statement.kind = "command"; statement.id = "goto"; } else


        {}

    }


}

