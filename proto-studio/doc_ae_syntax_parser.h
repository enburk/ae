#pragma once
#include <atomic>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    const std::set<str> keywords = 
    {
        "goto", "label",

        "if", "then", "else",

        "for", "in", "while", "do",

        "await", "yield", "return", "break", "continue", "throw", 

        "try", "throw", "catch", 

        "let", "type", "extends", "narrows", "implements", "extension", 

        "property", "function", "mutation", "operator", "precede", "succeed",

        "import", "self", "same",
        
        "assert", "invariant",

        "or", "and", "xor", "not",
    };

    array<str> subsup = {
    u8"₀", u8"₁", u8"₂", u8"₃", u8"₄", u8"₅", u8"₆", u8"₇", u8"₈", u8"₉", 
    u8"⁰", u8"¹", u8"²", u8"³", u8"⁴", u8"⁵", u8"⁶", u8"⁷", u8"⁸", u8"⁹"};
    array<str> digits = {"0","1","2","3","4","5","6","7","8","9"};
    array<str> hexadecimals = {"0","1","2","3","4","5","6","7","8","9",
    "A","B","C","D","E","F", "a","b","c","d","e","f"};

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
        array<token>::range_type input;
        
        report& log; std::atomic<bool>& cancel;

        auto output () { return bracketing(); }

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

                if (token.kind == "number")
                {
                    int dots = 0;
                    int base = 10;
                    if (token.text.ends_with(str(u8"₁₆"))) {
                        token.text.resize(token.text.size() -
                        str(u8"₁₆").size());
                        base = 16; }
                    if (token.text.ends_with(str(u8"₂"))) {
                        token.text.resize(token.text.size() -
                        str(u8"₂").size());
                        base = 2; }

                    for (auto c: aux::unicode::glyphs(token.text))
                    {
                        if (c == ".") {
                        if (++dots > 1 or base != 10) {
                        log.error(&token, "invalid number");
                        break; } else continue; }

                        if (base == 10 and not digits.contains(c)) {
                        token.kind = "name";
                        break; }

                        if (base ==  2 and c != "0" and c != "1"
                        or  base == 16 and not hexadecimals.contains(c)) {
                        log.error(&token, "invalid number");
                        break; }
                    }
                }

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

