#pragma once
#include <set>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    static const std::set<str> keywords = 
    {
        "using", "keywords",

        "for", "each", "in", "while", "until", "do", "break", "continue", 
            
        "if", "then", "else", "select",

        "asynch", "await", "yield",

        "call", "goto", "return", "ignore",

        "true", "false", "and", "or", "xor", "not", "bitwise", 

        "operator", "class",
        "function", "union",
        "mutation", "type",
        "variable", "this",
        "constant",
        "requires",

        "byte",
        "boolean",
        "natural",
        "natural.16", "natural.16.BE", "natural.16.LE",
        "natural.32", "natural.32.BE", "natural.32.LE",
        "natural.64", "natural.64.BE", "natural.64.LE",
        "integer",
        "integer.16", "integer.16.BE", "integer.16.LE",
        "integer.32", "integer.32.BE", "integer.32.LE",
        "integer.64", "integer.64.BE", "integer.64.LE",
        "rational",
        "real",
        "real.32",
        "real.64",
        "real.80",
    };

    struct statementor
    {
        token * last_token = nullptr;
        using error = std::logic_error;

        array<statement> output;
        deque<element> elements;
        array<element> && input; report & log; statementor(
        array<element> && input, report & log) : input(std::move(input)), log(log)
        {
            try
            {
                for (auto && element : input)
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

                    if (log.errors.size() > 0) break;

                    output += std::move(s);
                }
            }
            catch (const std::exception & e) { log.error(last_token, e.what()); }
        }

        statement read_statement ()
        {
            str schema; array<token*> names;

            for (auto & e : elements)
            {
                schema += " ";

                if (e.name == "{}") schema += "{}"; else
                if (e.kind == "()") schema += "()"; else
                if (e.opening->text == "=" ) schema += "=";  else
                if (e.opening->text == "," ) schema += ",";  else
                if (e.opening->text == ":" ) schema += ":";  else
                if (e.opening->text == "::") schema += "::"; else
                if (e.opening->text == ":=") schema += ":="; else
                if (e.opening->text == (char*)(u8"→")) schema += "->"; else
                if (e.opening->kind == "name" and keywords.contains(e.opening->text)) {
                    schema += e.opening->text; e.opening->kind = "keyword"; } else
                    schema += e.opening->kind;
            
                if (schema.ends_with(" =" )) break;
                if (schema.ends_with(" :=")) break;

                if (schema.ends_with(" name")) names += e.opening;
                if (schema.ends_with(" names , name")) schema.truncate(schema.size() - 7);
                if (schema.ends_with(" name , name")){ schema.truncate(schema.size() - 7);
                    schema += "s";
                }
            }
            if (schema != "") schema.erase(0); // leading " "

            if (schema.starts_with("using "))
            {
                pragma s;
                s.title = read_token();
                s.param = read_literal();
                return statement{std::move(s)};
            }

            if (schema.starts_with("if "))
            {
                conditional s;
                s.title = read_token();
                s.condition = read_expression_until("then");
                s.then_body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema.starts_with("else "))
            {
                conditional s;
                s.title = read_token();
                s.else_body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema.starts_with("for "))
            {
                loop_for s;
                s.title = read_token();
                read("each"); s.names = read_list_of_names();
                read("in"); s.range = read_named_pack();
                read("do"); s.body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema.starts_with("while "))
            {
                loop_while s;
                s.title = read_token();
                s.condition = read_expression_until("do");
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema.starts_with("until "))
            {
                loop_while s;
                s.title = read_token();
                s.condition = read_expression_until("do");
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }

            if (schema == "name {}")
            {
                declaration s;
                s.kind = "singleton";
                s.names = read_list_of_names();
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }

            if (schema.starts_with("name : type ="))
            {
                declaration s;
                s.kind = "type";
                s.names = read_list_of_names();
                read(":"); read("type"); read("=");
                s.body = read_statement_or_body();
                return statement{std::move(s), schema};
            }

            if (schema.starts_with("name :")
            or  schema.starts_with("names :"))
            {
                declaration s;
                s.kind = "variable";
                s.names = read_list_of_names(); read(":");
                s.type = read_named_pack();
                if (not elements.empty()) { read("=");
                    s.body = read_statement_or_body();
                }
                return statement{std::move(s), schema};
            }

            if (schema.starts_with("function ")
            or  schema.starts_with("mutation "))
            {
                subroutine s;
                s.title = read_token();
                s.kind = s.title->text;
                s.name = read_name();
                s.parameters = read_parameters();

                if (schema.starts_with("function name () ->")
                or  schema.starts_with("mutation name () ->")) {
                    read((char*)(u8"→"));
                    s.type = read_named_pack();
                }
                read("=");
                s.body = read_statement_or_body();
                return statement{std::move(s), schema};
            }

            if (schema.starts_with("operator "))
            {
                schema.replace_all(" name "  , " x ");
                schema.replace_all(" symbol ", " x ");

                subroutine s; s.title = read("operator");

                if (schema == "operator () =")
                {
                    s.kind = "operator ()";
                    s.parameters = read_parameters(); read("=");
                    s.body = read_statement_or_body();
                    return statement{std::move(s)};
                }
                if (schema == "operator x () =")
                {
                    s.kind = "prefix operator";
                    s.name = read_token();
                    s.parameters = read_one_parameter(); read("=");
                    s.body = read_statement_or_body();
                    return statement{std::move(s)};
                }
                if (schema == "operator () x =")
                {
                    s.kind = "postfix operator";
                    s.parameters = read_one_parameter();
                    s.name = read_token(); read("=");
                    s.body = read_statement_or_body();
                    return statement{std::move(s)};
                }
                if (schema == "operator () x () =")
                {
                    s.kind = "binary operator";
                    s.parameters = read_one_parameter(); s.name = read_token();
                    s.parameters += read_one_parameter(); read("=");
                    s.body = read_statement_or_body();
                    return statement{std::move(s)};
                }

                if (elements.size() == 0)
                throw error ("expected parameter, name or symbol");
                throw error ("expected '='");
            }

            return statement{read_expression(), schema};
        }

        str next () { return
            elements.empty() ? "" :
            elements.front().opening->text;
        }

        token* read (str s)
        {
            token* token = read_token("expected '" + s + "'");
            if (token->text != s) throw error("expected '" + s + "'");
            return token;
        }

        token* read_name ()
        {
            token* token = read_token("expected name");
            if (token->kind != "name") throw error("expected name, not " + token->kind);
            return token;
        }

        token* read_literal ()
        {
            token* token = read_token("expected literal");
            if (token->kind != "literal"   ) throw error("expected literal");
            if (token->text.size () < 2    ) throw error("expected \"\"");
            if (token->text.front() != '\"') throw error("expected \"\"");
            if (token->text.back () != '\"') throw error("expected \"\"");
            return token;
        }

        token* read_token (str what = "")
        {
            if (what != "" && elements.empty()) throw error(what);
            token* token = last_token = elements[0].opening;
            elements.pop_front();
            return token;
        }

        array<token*> read_list_of_names ()
        {
            array<token*> names;
            names += read_name();
            while (not elements.empty()) {
                if (next() != ",") break; read(",");
                names += read_name();
            }
            return names;
        }

        brackets read_brackets ()
        {
            if (elements.empty() or
                elements.front().kind != "()")
                throw error("expected brackets");

            brackets brackets;
            brackets.opening = elements.front().opening;
            brackets.closing = elements.front().closing;

            auto eee =
            elements.front().elements;
            elements.pop_front();
            
            for (auto & ee : eee) {
                auto eedeque = deque(ee.elements);
                std::swap(elements, eedeque); brackets.list += read_expression();
                std::swap(elements, eedeque);
            }
            return brackets;
        }

        named_pack read_named_pack ()
        {
            named_pack pack;

            if (elements.empty()) throw error("expected name or '::'");

            while (not elements.empty())
            {
                if (next() == "::")
                {
                    pack.units += named_unit{};
                    pack.units.back().coloncolon = read("::");
                    continue;
                }
                if (elements.front().opening->kind == "name"
                or  elements.front().opening->kind == "keyword") // integer etc.
                {
                    if (pack.units.empty()) pack.units += named_unit{};
                    if (pack.units.back().identifier != nullptr) break;
                    pack.units.back().identifier = read_token();
                    continue;
                }
                if (elements.front().kind == "()")
                {
                    if (pack.units.empty()) break;
                    if (pack.units.back().identifier == nullptr) break;
                    pack.units.back().arguments += read_brackets();
                    continue;
                }
                break;
            }

            if (pack.units.empty() or
                pack.units.back().identifier == nullptr)
                throw error("expected name");

            return pack;
        }

        parameter read_parameter ()
        {
            parameter p; p.name = read_name();
            if (next() == ":") { read(":"); p.type  = read_named_pack(); }
            if (next() == "=") { read("="); p.value = read_expression(); }
            if (not elements.empty()) { read_token(); throw error("unexpected token"); }
            return p;
        }

        array<parameter> read_parameters ()
        {
            array<parameter> parameters;

            if (elements.empty() ||
                elements.front().name != "()")
                throw error("expected parameters");

            auto eee =
            elements.front().elements;
            elements.pop_front();
            
            for (auto & ee : eee) {
                auto eedeque = deque(ee.elements);
                std::swap(elements, eedeque); parameters += read_parameter();
                std::swap(elements, eedeque);
            }
            return parameters;
        }

        array<parameter> read_one_parameter ()
        {
            array<parameter> parameters = read_parameters();
            if (parameters.size() != 1) throw error
            ("expected exactly one parameter");
            return parameters;
        }

        array<statement> read_statement_or_body ()
        {
            array<statement> body;

            if (elements.size() == 0) throw error("expected statement");
            if (elements.size() == 1 and elements.front().name == "{}")
            {
                body = statementor(std::move(
                elements.front().elements), log).output;
                elements.pop_front();
            }
            else body += statement{read_expression()};

            if (body.empty())
                body += statement{};
            else
            if (body.size() == 1 and
                aux::got<expression>(body[0].variant) and aux::got<symbol>(
                std::get<expression>(body[0].variant).variant) and std::get<symbol>(
                std::get<expression>(body[0].variant).variant).token->text == "...")
                body.clear();

            return body;
        }

        expression read_expression () { return read_expression_until(""); }
        expression read_expression_until (str until)
        {
            operation o;

            while (not elements.empty())
            {
                if (next() == until) {
                    read_token();
                    until = "";
                    break;
                }

                str kind = elements.front().opening->kind;
                if (kind == "number" ) o.operands += expression{number {read_token()}}; else
                if (kind == "symbol" ) o.operands += expression{symbol {read_token()}}; else
                if (kind == "literal") o.operands += expression{literal{read_token()}}; else

                if (elements.front().kind == "()")
                    o.operands += expression{read_brackets()};
                else
                if (next() == "if")
                {
                    expression_if e;
                    e.title = read("if");
                    e.condition += read_expression_until("then");
                    e.then_body += read_expression_until("else");
                    e.else_body += read_expression_until("");
                    o.operands += expression{e};
                }
                else
                if (next() == "for")
                {
                    expression_for e;
                    e.title = read("for");
                    read("each"); e.names = read_list_of_names();
                    read("in"); e.range += read_expression();
                    o.operands += expression{e};
                }
                else o.operands += expression{read_named_pack()};
            }

            if (until != "") throw error("expected '" + until + "'");
            if (o.operands.size() == 0) throw error("expected expression");
            if (o.operands.size() == 1) return o.operands[0];
            return expression{o};
        }
    };
}

