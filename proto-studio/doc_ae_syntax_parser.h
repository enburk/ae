#pragma once
#include <atomic>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    const std::set<str> keywords = 
    {
        "for", "in", "while", "do",

        "await", "yield", "return", "break", "continue", 

        "type", "extends", "subset.of", "extension", 

        "function", "mutation", "operator",
    };

    struct element
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<element> elements;

        str print (bool details = false)
        {
            str s;
            if (opening) s +=
                opening->text;

            if (details
            and not elements.empty()) {
            for (auto& e: elements)
            s += e.print() + " ";
            s.truncate(); }

            if (closing) s +=
                closing->text;
            return s;
        }
    };

    struct parser
    {
        array<element> output;
        array<token>::range_type input; report& log; std::atomic<bool>& cancel; parser (
        array<token>::range_type input, report& log, std::atomic<bool>& cancel) :
        input(input), log(log), cancel(cancel)
        {
            output = 
            bracketing(
            );
        }

        auto bracketing (str closing = "") -> array<element>
        {
            auto close = [this](element& e)
            {
                if (not input.empty()) {
                e.closing = &input.front();
                input >>= 1; }

                else log.error(
                e.opening, "unclosed " + bold(
                e.opening->text));
            };

            array<element> output;
            
            while (not input.empty())
            {
                if (cancel) break;

                auto& token = input.front();

                if (token.text == "}" and closing == "}") break;
                if (token.text == ")" and closing == ")") break;
                if (token.text == "]" and closing == ")") break;
            
                input >>= 1;

                if (token.kind == "space"
                or  token.kind == "comment"
                or  token.text == "\n")
                    continue;

                if (token.text == ":")
                {
                    token.kind = "keyword";
                    if (output.empty()) {
                    log.error(&token, "unexpected " + bold(":"));
                    continue; }

                    auto& last = output.back().opening;
                    if (last->kind != "name") {
                    log.error(&token, "unexpected " + bold(":"));
                    continue; }

                    if (keywords.contains(last->text))
                    last->kind = "keyword"; else log.error(
                    last, "unknown keyword " + bold(
                    last->text));
                    continue;
                }

                if (token.text == "}") { log.error(&token, "unexpected " + bold("}")); } else
                if (token.text == ")") { log.error(&token, "unexpected " + bold(")")); } else
                if (token.text == "]") { log.error(&token, "unexpected " + bold("]")); }

                element e; e.opening = &token;

                if (token.text == "{") { e.elements = bracketing("}"); close(e); } else
                if (token.text == "(") { e.elements = bracketing(")"); close(e); } else
                if (token.text == "[") { e.elements = bracketing(")"); close(e); }

                output += e;
            }

            return output;
        }
    };
}

