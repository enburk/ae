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

                    if (log.errors.size() > 0) break;

                    //if (std::holds_alternative<operator_if>(output.back().variant))
                }
            }
            catch (const std::exception & e) { log.error(last_token, e.what()); }

            return output;
        }

        statement read_statement (deque && input)
        {
            assert(input.size() > 0);

            if (input[0].opening->text == "using")
            {
                pragma s;
                s.title = read_token(input); s.title->kind = "keyword";
                s.param = read_literal(input);
                return statement{std::move(s)};
            }

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
            if (input[0].opening->text == "for")
            {
                loop_for s;
                s.title = read_token(input); s.title->kind = "keyword";
                read_name(input, "each")->kind = "keyword";
                s.names = read_list_of_names(input);
                read_name(input, "in")->kind = "keyword";
                s.range = read_named_pack(input);
                read_name(input, "do")->kind = "keyword";
                s.body = read_statement_or_body(input);
                return statement{std::move(s)};
            }
            if (input[0].opening->text == "while")
            {
                loop_while s;
                s.title = read_token(input); s.title->kind = "keyword";
                s.condition = read_expression_until("do", input);
                s.body = read_statement_or_body(input);
                return statement{std::move(s)};
            }
            if (input[0].opening->text == "until")
            {
                loop_while s;
                s.title = read_token(input); s.title->kind = "keyword";
                s.condition = read_expression_until("do", input);
                s.body = read_statement_or_body(input);
                return statement{std::move(s)};
            }

            str schema; array<token*> names;

            for (auto & e : input)
            {
                schema += " ";

                if (e.name == "{}") schema += "{}"; else
                if (e.kind == "()") schema += "()"; else
                if (e.opening->text == "=") schema += "="; else
                if (e.opening->text == ":") schema += ":"; else
                if (e.opening->text == ":=") schema += ":="; else
                if (e.opening->text == (char*)(u8"→")) schema += "->"; else
                if (e.opening->kind == "name" and
                    keywords.find(e.opening->text) != keywords.end())
                    schema += e.opening->text; else
                    schema += e.opening->kind;
            
                if (schema.ends_with(" ="))  break;
                if (schema.ends_with(" :"))  break;
                if (schema.ends_with(" :="))  break;
                if (schema.ends_with(" name"))
                    names += e.opening;

                if (schema.ends_with(" names , name")) schema.truncate(schema.size() - 7);
                if (schema.ends_with(" name , name")){ schema.truncate(schema.size() - 7);
                    schema += "s";
                }
            }

            if (schema != "") schema.erase(0); // leading " "

            if (schema == "name {}")
            {
                declaration s;
                s.names = names;
                s.type.units += named_unit{.identifier=names[0]};
                s.kind = "singleton";
                s.body = proceed(std::move(input[1].elements));
                return statement{std::move(s)};
            }

            if (schema.starts_with("function ")
            or  schema.starts_with("mutation "))
            {
                subroutine s;
                s.title = read_token(input);
                s.title->kind = "keyword";
                s.kind = s.title->text;
                s.name = read_name(input);
                s.parameters = read_parameters(input);

                if (schema.starts_with("function name () -> ")
                or  schema.starts_with("mutation name () -> ")) {
                    read_symbol(input, (char*)(u8"→"));
                    s.type = read_named_pack(input);
                }

                read_symbol(input, "=");
                s.body = read_statement_or_body(input);

             // if (s.body.size() == 1 and std::holds_alternative<expression>
             //    (s.body[0].variant) and std::holds_alternative<operation>(std::get<expression>
             //    (s.body[0].variant).variant) and std::get<operation>(std::get<expression>
             //    (s.body[0].variant).variant).title->text == "...")
                if (s.body.size() == 1 and std::holds_alternative<expression>
                   (s.body[0].variant) and std::holds_alternative<named_pack>(std::get<expression>
                   (s.body[0].variant).variant) and std::get<named_pack>(std::get<expression>
                   (s.body[0].variant).variant).units.size() == 1 and std::get<named_pack>(std::get<expression>
                   (s.body[0].variant).variant).units[0].identifier->text == "...")
                   {s.body.clear(); s.external = true;}

                return statement{std::move(s)};
            }

            if (schema.starts_with("operator "))
            {
                schema.replace_all(" name "  , " x ");
                schema.replace_all(" symbol ", " x ");

                subroutine s;
                s.title = read_token(input);
                s.title->kind = "keyword";

                if (schema == "operator () =")
                {
                    s.kind = "operator ()";
                    s.parameters = read_parameters(input);
                    read_symbol(input, "=");
                    s.body = read_statement_or_body(input);
                    return statement{std::move(s)};
                }
                if (schema == "operator x () =")
                {
                    s.kind = "prefix operator";
                    s.name = read_token(input);
                    s.parameters = read_parameters(input);
                    if (s.parameters.size() != 1) throw error
                    ("expected exactly one parameter");
                    read_symbol(input, "=");
                    s.body = read_statement_or_body(input);
                    return statement{std::move(s)};
                }
                if (schema == "operator () x =")
                {
                    s.kind = "postfix operator";
                    s.parameters = read_parameters(input);
                    if (s.parameters.size() != 1) throw error
                    ("expected exactly one parameter");
                    s.name = read_token(input);
                    read_symbol(input, "=");
                    s.body = read_statement_or_body(input);
                    return statement{std::move(s)};
                }
                if (schema == "operator () x () =")
                {
                    s.kind = "binary operator";
                    s.parameters = read_parameters(input);
                    if (s.parameters.size() != 1) throw error
                    ("expected exactly one parameter");
                    s.name = read_token(input);
                    s.parameters += read_parameters(input);
                    if (s.parameters.size() != 2) throw error
                    ("expected exactly one parameter");
                    read_symbol(input, "=");
                    s.body = read_statement_or_body(input);
                    return statement{std::move(s)};
                }

                if (input.size() == 0)
                throw error ("expected parameter, name or symbol");
                throw error ("expected '='");
            }

            return statement{read_expression_until("", input)};
        }

        token* read_token (deque & input, str what = "")
        {
            if (what != "" && input.empty()) throw error(what);
            token* token = last_token = input[0].opening;
            input.pop_front();
            return token;
        }

        token* read_symbol (deque & input, str symbol)
        {
            token* token = read_token(input, "expected '" + symbol + "'");
            if (token->text != symbol) throw error("expected '" + symbol + "'");
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

        token* read_literal (deque & input)
        {
            token* token = read_token(input, "expected literal");
            if (token->kind != "literal") throw error("expected literal");
            if (token->text.size() < 2) throw error("expected \"\"");
            if (token->text.front() != '\"') throw error("expected \"\"");
            if (token->text.back() != '\"') throw error("expected \"\"");
            return token;
        }

        array<token*> read_list_of_names (deque & input)
        {
            array<token*> names;
            names += read_name(input);
            while (input.size() > 0) {
                if (input[0].opening->text != ",") break;
                read_token(input);
                names += read_name(input);
            }
            return names;
        }

        parameter read_parameter (deque & input)
        {
            parameter parameter;
            parameter.name = read_name(input);
            if (parameter.name->text == "this")
                parameter.name->kind = "keyword";
            if (input.size() == 0) return parameter;
            read_name(input, ":");
            parameter.type = read_named_pack(input);
            return parameter;
        }

        array<parameter> read_parameters (deque & input)
        {
            if (input.size() == 0 || input[0].name != "()")
                throw error("expected parameters");

            array<parameter> parameters;

            for (auto && e : input[0].elements) {
                auto elements = deque(std::move(e.elements));
                parameters += read_parameter(elements);
            }

            input.pop_front();
            return parameters;
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

        named_pack read_named_pack (deque & input)
        {
            named_pack pack; pack.units += named_unit{};

            if (input.size() == 0) throw error("expected name");

            while (input.size() > 0)
            {
                auto & last = pack.units.back();

                if (input[0].opening->text == "::")
                {
                    if (last.identifier == nullptr) break;
                    if (last.coloncolon != nullptr) pack.units += named_unit{};
                    pack.units.back().coloncolon = read_token(input);
                }
                if (input[0].opening->kind == "name")
                {
                    if (last.identifier != nullptr) break;
                    last.identifier = read_token(input);
                }
                if (input[0].kind == "()")
                {
                    if (last.identifier == nullptr) break;
                    brackets b;
                    b.opening = input[0].opening;
                    b.closing = input[0].closing;
                    b.list = proceed(std::move(input[0].elements));
                    last.parameters += std::move(b);
                    input.pop_front();
                }
                break;
            }

            if (pack.units.back().identifier == nullptr)
                throw error("expected name");

            return pack;
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
                    read_name(input, "each")->kind = "keyword";
                    e.names = read_list_of_names(input);
                    read_name(input, "in"  )->kind = "keyword";
                    e.range += read_expression_until("", input);
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
    };
}

