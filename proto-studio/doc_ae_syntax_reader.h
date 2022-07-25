#pragma once
#include "doc_ae_syntax.h"
#include "doc_ae_syntax_parser.h"
namespace doc::ae::syntax
{
    struct reader
    {
        token * last_token = nullptr;

        auto error(str what) {
         log.error(last_token, what);
         throw what; }

        deque<element> input; report& log; std::atomic<bool>& cancel; reader(
        array<element> input, report& log, std::atomic<bool>& cancel) :
            input(deque(std::move(input))), log(log), cancel(cancel) {}

        str next () { return
            input.empty() ? "" :
            input.front().opening->text; }

        str next_kind () { return
            input.empty() ? "" :
            input.front().opening->text == "{" ? "()" :
            input.front().opening->text == "(" ? "()" :
            input.front().opening->text == "[" ? "()" :
            input.front().opening->kind; }

        void expected (str s) {
            if (input.empty()) error("next expected " + s);
            last_token = input.front().opening; input.pop_front(); // next changed
            error("expected " + bold(s)); }

        void unexpected () {
            if (input.empty()) error("unexpected: nosense");
            last_token = input.front().opening; input.pop_front(); // next changed
            error("unexpected " + bold(last_token->text)); }

        auto read (str s = "") -> token* {
            if (input.empty()) error("next expected " + s);
            last_token = input.front().opening; input.pop_front(); // next changed
            if (s != "" and s != last_token->text) error("expected " + bold(s));
            return last_token; }

        auto read_name () -> token* {
            if (input.empty()) error("next expected name");
            last_token = input.front().opening; input.pop_front(); // next changed
            if (last_token->kind != "name") error("expected name, not " + last_token->kind);
            return last_token; }

        auto read_name_or_symbol () -> token* {
            if (input.empty()) error("next expected name or symbol");
            last_token = input.front().opening; input.pop_front(); // next changed
            if (last_token->kind != "name" and last_token->kind != "symbol")
            error("expected name or symbol, not " + last_token->kind);
            return last_token; }

        auto read_namepack ()
        {
            array<nameunit> names;
            while (not input.empty())
            {
                if (next() == "::")
                {
                    if (not names.empty()
                    and not names.back().identifier)
                    expected("name, not ::"); // will throw
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
                    if (names.empty()) expected("name or ::");
                    if (names.back().identifier == nullptr) break;
                    names.back().argss += read_brackets();
                    continue;
                }
                break;
            }
            if (names.empty()
            or  names.back().identifier == nullptr)
            expected("name or ::"); // will throw
            // on attempt to read next token and
            // if there is one will highlight it
            return namepack{names};
        }

        auto read_brackets () -> brackets
        {
            if (next_kind() != "()")
            error("expected brackets");

            brackets brackets;
            brackets.opening = input.front().opening;
            brackets.closing = input.front().closing;
            brackets.list = reader(std::move(
            input.front().elements), log, cancel).
            read_list_of_expressions();
            input.pop_front();
            return brackets;
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
            p.typexpr = read_namepack();
            if (next_kind() == "name")
            p.name = read_name();
            if (next() == "=") { read("=");
            p.value = read_expression(); }
            return p;
        }

        auto read_list_of_parameters ()
        {
            array<parameter> parameters;
            parameters += read_parameter(); while (next() == ",") { read(",");
            parameters += read_parameter(); }
            if (not input.empty())
            expected("next parameter");
            return parameters;
        }

        auto read_parameters ()
        {
            if (next_kind() != "()")
            error("expected parameters");

            parameters parameters;
            parameters.opening = input.front().opening;
            parameters.closing = input.front().closing;
            parameters.list = reader(std::move(
            input.front().elements), log, cancel).
            read_list_of_parameters();
            input.pop_front();
            return parameters;
        }

        auto read_one_parameter ()
        {
            auto parameters = read_parameters();
            if (parameters.list.size() != 1)
            error("expected exactly one parameter");
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
            return read_namepack(); }
            return namepack{};
        }

        auto read_expression_until (str until) -> expression
        {
            operands o;
            while (not input.empty())
            {
                if (cancel) break;

                if (next() == ";"
                or  next() == ",")
                    break;

                if (next() == until) {
                    read(until);
                    until = "";
                    break; }

                if (next_kind() == "keyword") { read();
                error("unexpeted keyword in expression"); }

                if (next() == "::")
                    o.list += expression{
                    read_namepack()};
                    else

                if (next_kind() == "()")
                    o.list += expression{
                    read_brackets()};
                    else

                if (next_kind() == "number" or
                    next_kind() == "symbol" or
                    next_kind() == "literal")
                    o.list += expression{
                    terminal{read(next())}};
                    else

                    o.list += expression{
                    read_namepack()};
            }
            if (until != "") error("expected " + until);
            if (o.list.size() == 0) error("expected expression");
            if (o.list.size() == 1) return o.list[0];
            return expression{o};
        }
        auto read_expression () -> expression { return
             read_expression_until(""); }

        auto read_list_of_expressions () -> array<expression>
        {
            array<expression> expressions;
            expressions += read_expression(); while (next() == ",") { read(",");
            expressions += read_expression(); }
            return expressions;
        }
    };
}

