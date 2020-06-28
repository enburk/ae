#pragma once
#include <set>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    static const std::set<str> pragmas = 
    {
        "using", "keywords"
    };
    static const std::set<str> keywords = 
    {
        "for", "while", "until", "do", "break", "continue", 
            
        "if", "then", "else", "select",

        "asynch", "await", "yield",

        "ignore", "call", "goto", "return",

        "operator", "class",
        "function", "union",
        "mutation", "type",
        "variable",
        "constant",
        "requires",

        "byte",
        "boolean",
        "natural",
        "integer",
        "rational",
        "real",
    };

    struct statementor
    {
        statementor (report & log) : log(log) {}

        report & log; token * last_token = nullptr;

        using deque = deque<element>;
        using error = std::logic_error;

        auto proceed (array<element> && input) -> array<statement>
        {
            array<statement> output;

            try
            {
                for (auto && es : input)
                {
                    output += read_statement(deque(es.elements));

                    if (log.messages.size() > 0) break;

                    //if (std::holds_alternative<operator_if>(output.back().variant))
                }
            }
            catch (const std::exception & e) { log.error(last_token, e.what()); }

            return output;
        }

        statement read_statement (deque && input)
        {
            assert(input.size() > 0);

            if (input[0].opening->text == "if")
            {
                conditional s;
                s.title = read_token(input); s.title->kind = "keyword";
                s.condition = read_expression_until("then", input);
                s.then_body = read_statement_or_body(input);
                return statement{std::move(s)};
            }
            if (input[0].opening->text == "else")
            {
                conditional s;
                s.title = read_token(input); s.title->kind = "keyword";
                s.else_body = read_statement_or_body(input);
                return statement{std::move(s)};
            }
/*
        auto & elements = statement.elements;

        if (elements.size() == 0) return;
        if (elements[0].opening->text == "if"
        or  elements[0].opening->text == "else"
        or  elements[0].opening->text == "for"
        or  elements[0].opening->text == "while"
        or  elements[0].opening->text == "do"
        or  elements[0].opening->text == "goto"
        ) {
            statement.kind = "operator";
            statement.name = elements[0].opening->text;
            statement.opening = elements[0].opening;
            elements.erase(0);
        }

        str schema; array<str> names;

        for (auto & e : elements)
        {
            if (e.kind == "{}") schema += "{}"; else
            if (e.kind == "()") schema += "()"; else
            if (e.opening->kind == "symbol" or
                e.opening->kind == "name" and
                keywords.find(e.opening->text) != keywords.end())
                schema += e.opening->text; else
                schema += e.opening->kind;
            
            if (schema.ends_with(" name"))
                names += e.opening->text;

            schema += " ";
        }
        schema.truncate();

        auto & kind = statement.kind;
        auto & name = statement.name;

        if (schema == "name {}") { kind = "singleton"; name = names[0]; } else

        if (schema == "operator id = {}") { statement.kind = "operator"; statement.id = ids[0]; } else
        if (schema == "function id = {}") { statement.kind = "function"; statement.id = ids[0]; } else
        if (schema == "mutation id = {}") { statement.kind = "mutation"; statement.id = ids[0]; } else
        if (schema == "variable id = {}") { statement.kind = "variable"; statement.id = ids[0]; } else
        if (schema == "constant id = {}") { statement.kind = "constant"; statement.id = ids[0]; } else



        {}

    }
*/

            throw error("invalid statement");
        }

        token* read_token (deque & input, str what = "")
        {
            if (what != "" && input.empty()) throw error(what);
            token* token = last_token = input[0].opening;
            input.pop_front();
            return token;
        }

        token* read_name (deque & input, str name)
        {
            token* token = read_token(input, "expected '" + name + "'");
            if (token->text != name) throw error("expected '" + name + "'");
            return token;
        }

        token* read_name (deque & input)
        {
            token* token = read_token(input, "expected name");
            if (token->kind != "name") throw error("expected name");
            return token;
        }

        expression read_expression_until (str until, deque & input)
        {
            operation o;

            while (input.size() > 0)
            {
                if (input[0].kind == "()")
                {
                    brackets b;
                    b.opening = input[0].opening;
                    b.closing = input[0].closing;
                    b.list = proceed(std::move(input[0].elements));
                    o.operands += expression{std::move(b)};
                    input.pop_front();
                    continue;
                }

                token* token = read_token(input);

                if (token->text == until) {
                    token->kind = "keyword";
                    until = "";
                    break;
                }

                if (token->kind == "number") o.operands += expression{number{token}}; else
                if (token->kind == "literal") o.operands += expression{literal{token}}; else
                
                if (token->text == "if")
                {
                    expression_if e;
                    e.title = token; e.title->kind = "keyword";
                    e.condition += read_expression_until("then", input);
                    e.then_body += read_expression_until("else", input);
                    e.else_body += read_expression_until("", input);
                    o.operands += expression{e};
                }
                else
                if (token->text == "for")
                {
                    expression_for e;
                    e.title = token; e.title->kind = "keyword";
                    token = read_name(input, "each"); token->kind = "keyword";
                    e.index = read_name(input);
                    token = read_name(input, "in"); token->kind = "keyword";
                    e.container += read_expression_until("", input);
                    o.operands += expression{e};
                }
                else
                if (token->text == "::")
                {
                    named_unit e;
                    named_pack ee;
                    e.coloncolon = token;
                    e.identifier = read_name(input);
                    ee.units += e;
                    o.operands += expression{ee};
                }
                else // symbol or name
                {
                    named_unit e;
                    named_pack ee;
                    e.identifier = token;
                    ee.units += e;
                    o.operands += expression{ee};
                }
            }

            if (until != "") throw error("expected '" + until + "'");
            if (o.operands.size() == 0) throw error("expected expression");
            if (o.operands.size() == 1) return o.operands[0];
            return expression{o};
        }

        array<statement> read_statement_or_body (deque & input)
        {
            if (input.size() == 0) throw error("expected statement");

            if (input.size() == 1 && input[0].name == "{}")
                return proceed(std::move(input[0].elements));

            array<statement> body;
            body += read_statement(std::move(input));
            return body;
        }
    };
}

