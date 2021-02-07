#pragma once
#include "doc_ae_syntax.h"
#include "doc_ae_syntax_parser.h"
namespace doc::ae::syntax
{
    struct reader
    {
        token * last_token = nullptr;
        using error = std::logic_error;

        deque<element> input; report& log; reader(
        array<element> input, report& log) :
            input(deque(input)), log(log) {}

        str next () { return
            input.empty() ? "" :
            input.front().opening->text;
        }
        str next_kind () { return
            input.empty() ? "" :
            input.front().opening->kind;
        }

        auto read (str s) -> token*
        {
            if (input.empty()) throw error("next expected " + s);
            token* token = last_token = input.front().opening; input.pop_front();
            if (token->text != s) throw error("expected " + s);
            return token;
        }
        auto read_name () -> token*
        {
            if (input.empty()) throw error("next expected name");
            token* token = last_token = input.front().opening; input.pop_front();
            if (token->kind != "name") throw error("expected name, not " + token->kind);
            return token;
        }

        auto read_brackets ()
        {
            if (input.empty() or
                input.front().kind != "()")
                throw error("expected brackets");

            brackets brackets;
            brackets.opening = input.front().opening;
            brackets.closing = input.front().closing;

            auto && eee = std::move(
            input.front().elements);
            input.pop_front();
            
            for (auto && ee : eee)
                brackets.list += reader(std::move(ee.elements), log)
                    .read_expression();

            return brackets;
        }

        auto read_namepack ()
        {
            array<nameunit> names;

            while (not input.empty())
            {
                if (next() == "::")
                {
                    names += nameunit{.coloncolon = read("::")};
                    continue;
                }
                if (next_kind() == "name")
                {
                    if (names.empty()) names += nameunit{};
                    if (names.back().identifier != nullptr) break;
                    names.back().identifier = read_name();
                    continue;
                }
                if (next_kind() == "()")
                {
                    if (names.empty()) break;
                    if (names.back().identifier == nullptr) break;
                    names.back().args += read_brackets();
                    continue;
                }
                break;
            }

            if (names.empty() or
                names.back().identifier == nullptr) {
                read("name or ::"); // will throw
                // on attempt to read next token and
                // if there is one then highlight it
            }

            return namepack{names};
        }

        auto read_list_of_names ()
        {
            array<token*> names;
            names += read_name(); while (next() == ",") { read(",");
            names += read_name(); }
            return names;
        }

        auto read_list_of_namepacks ()
        {
            array<namepack> names;
            names += read_namepack(); while (next() == ",") { read(",");
            names += read_namepack(); }
            return names;
        }

        auto read_optional_type ()
        {
            if (next()
                ==   (char*)(u8"→")) {
                read((char*)(u8"→"));
                return read_namepack();
            }
            return namepack{};
        }

        parameter read_parameter ()
        {
            parameter p;

            if (input.size() >= 3 and
                input[0].opening->kind == "name" and
                input[1].opening->text == ":")
            {
                p.name = read_name();
                if (next() == ":") { read(":"); p.type  = read_named_pack(); }
                if (next() == "=") { read("="); p.value = read_expression(); }
            }
            else
            if (input.size() >= 2)
            {
                p.type = read_named_pack();
                p.name = read_name();
                if (next() == "=") { read("="); p.value = read_expression(); }
            }
            else
            {
                p.name = read_name();
            }

            if (not input.empty()) { read_token(); throw error("unexpected token"); }

            return p;
        }

        parameters read_parameters ()
        {
            parameters parameters;

            if (input.empty() ||
                input.front().kind != "()")
                throw error("expected parameters");

            parameters.opening = input.front().opening;
            parameters.closing = input.front().closing;

            auto eee =
            input.front().input;
            input.pop_front();
            
            for (auto & ee : eee) {
                auto eedeque = deque(ee.input);
                std::swap(input, eedeque); parameters.list += read_parameter();
                std::swap(input, eedeque);
            }
            return parameters;
        }

        auto read_one_parameter ()
        {
            auto parameters = read_parameters();
            if (parameters.list.size() != 1) throw error
            ("expected exactly one parameter");
            return parameters[0];
        }

        auto read_expression () -> expression { return read_expression_until(""); }
        auto read_expression_until (str until) -> expression
        {
            operation o;

            while (not input.empty())
            {
                if (next() == until) {
                    read(until);
                    until = "";
                    break;
                }

                str kind = next_kind();
                if (kind == "number" ) o.operands += expression{number {read_token()}}; else
                if (kind == "symbol" ) o.operands += expression{symbol {read_token()}}; else
                if (kind == "literal") o.operands += expression{literal{read_token()}}; else

                if (input.front().kind == "()")
                    o.operands += expression{read_brackets()};
                
                else try { o.operands += expression{read_named_pack()}; }
                catch(...) { throw error("expression should start with name"); }
            }

            if (until != "") throw error("expected " + until);
            if (o.operands.size() == 0) throw error("expected expression");
            if (o.operands.size() == 1) return o.operands[0];
            return expression{o};
        }
    };
}

