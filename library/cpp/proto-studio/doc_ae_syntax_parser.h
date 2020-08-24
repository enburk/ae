#pragma once
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct parser
    {
        array<element> output;
        array<token> & input; report & log; parser (
        array<token> & input, report & log) : input(input), log(log)
        {
            deque<token*> tokens;
            for (auto & token : input)
                if (token.kind != "space" &&
                    token.kind != "comment"&&
                    token.text != "\n")
                    tokens += &token;

            output = 
            elementing(
            bracketing(
            tokens));
        }

        auto bracketing (deque<token*> & input, str closing = "") -> array<element>
        {
            auto close = [this, &input](element & e)
            {
                if (input.size() > 0) {
                    auto token = input.front();
                    input.pop_front();
                    e.closing = token;
                    e.name = e.opening->text + e.closing->text;
                    e.kind = e.name == "{}" ? "{}" : "()";
                }
                else log.error(e.opening, "unclosed '" + e.opening->text + "'");
            };

            array<element> output; while (input.size() > 0)
            {
                auto token = input.front();

                if (token->text == "}" && closing == "}") break;
                if (token->text == ")" && closing == ")") break;
                if (token->text == "]" && closing == ")") break;
            
                input.pop_front();

                if (token->text == "}") { log.error(token, "unexpected '}'"); } else
                if (token->text == ")") { log.error(token, "unexpected ')'"); } else
                if (token->text == "]") { log.error(token, "unexpected ']'"); }

                element e; e.opening = token;

                if (token->text == "{") { e.elements = bracketing(input, "}"); close(e); } else
                if (token->text == "(") { e.elements = bracketing(input, ")"); close(e); } else
                if (token->text == "[") { e.elements = bracketing(input, ")"); close(e); }

                output += e;
            }
            return output;
        }

        auto elementing (array<element> input) -> array<element>
        {
            array<element> output; element s;

            if (log.errors.size() > 0) return output;
        
            for (auto && e : input)
            {
                if (e.opening->text == ";")
                {
                    if (s.elements.empty()) continue;
                    output += std::move(s);
                    s = element{};
                }
                else
                if (e.name == "{}")
                {
                    e.elements = elementing(e.elements);
                    s.elements += std::move(e);
                    output += std::move(s);
                    s = element{};
                }
                else
                if (e.kind == "()")
                {
                    e.elements = argumenting(e.elements);
                    s.elements += std::move(e);
                }
                else s.elements += std::move(e);
            }

            if (not s.elements.empty())
                output += std::move(s);
    
            return output;
        }

        auto argumenting (array<element> input) -> array<element>
        {
            array<element> output; element s;

            for (auto && e : input)
            {
                if (e.opening->text == ";")
                {
                    log.error(e.opening, "unexpected ';'");
                }
                if (e.opening->text == ",")
                {
                    output += std::move(s);
                    s = element{};
                }
                else
                if (e.name == "{}")
                {
                    e.elements = elementing(e.elements);
                    s.elements += std::move(e);
                }
                else
                if (e.kind == "()")
                {
                    e.elements = argumenting(e.elements);
                    s.elements += std::move(e);
                }
                else s.elements += e;
            }

            if (not s.elements.empty())
                output += s;
    
            return output;
        }
    };
}

