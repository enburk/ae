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

        auto read (str s = "") -> token*
        {
            if (input.empty()) throw error("next expected " + s);
            token* token = last_token = input.front().opening; input.pop_front();
            if (s != "" && s != token->text) throw error("expected " + s);
            return token;
        }
        auto read_name () -> token*
        {
            if (input.empty()) throw error("next expected name");
            token* token = last_token = input.front().opening; input.pop_front();
            if (token->kind != "name" && token->kind != "symbol")
            throw error("expected name or symbol, not " + token->kind);
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

        parameter read_parameter ()
        {
            parameter p;

            if (input.size() >= 3 and
                input[0].opening->kind == "name" and
                input[1].opening->text == ":")
            {
                p.name = read_name();
                if (next() == ":") { read(":"); p.type  = read_namepack(); }
                if (next() == "=") { read("="); p.value = read_expression(); }
            }
            else
            if (input.size() >= 2)
            {
                p.type = read_namepack();
                p.name = read_name();
                if (next() == "=") { read("="); p.value = read_expression(); }
            }
            else
                p.name = read_name();

            return p;
        }

        auto read_parameters ()
        {
            if (input.empty() ||
                input.front().kind != "()")
                throw error("expected parameters");

            parameters parameters;
            parameters.opening = input.front().opening;
            parameters.closing = input.front().closing;

            auto && eee = std::move(
            input.front().elements);
            input.pop_front();
            
            for (auto && ee : eee)
                parameters.list += reader(std::move(ee.elements), log)
                    .read_parameter();

            return parameters;
        }

        auto read_one_parameter ()
        {
            auto parameters = read_parameters();
            if (parameters.list.size() != 1) throw error
            ("expected exactly one parameter");
            return parameters.list[0];
        }

        auto read_optional_args ()
        {
            return next_kind() == "()" ?
                read_parameters() :
                parameters{};
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

        auto read_expression () -> expression { return read_expression_until(""); }
        auto read_expression_until (str until) -> expression
        {
            operands o;

            while (not input.empty())
            {
                if (next() == until) {
                    read(until);
                    until = "";
                    break;
                }

                str kind = next_kind();
                if (kind == "number" ) o.list += expression{terminal{read(next())}}; else
                if (kind == "symbol" ) o.list += expression{terminal{read(next())}}; else
                if (kind == "literal") o.list += expression{terminal{read(next())}}; else

                if (input.front().kind == "()")
                    o.list += expression{read_brackets()};
                else
                    o.list += expression{read_namepack()};
            }

            if (until != "") throw error("expected " + until);
            if (o.list.size() == 0) throw error("expected expression");
            if (o.list.size() == 1) return o.list[0];
            return expression{o};
        }
    };
}

