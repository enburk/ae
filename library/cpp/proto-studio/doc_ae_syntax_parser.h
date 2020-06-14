#pragma once
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct parser
    {
        report & log;

        parser (report & log) : log(log) {}

        array<element> bracketing (deque<token*> & input, str closing)
        {
            auto close = [this, &input](brackets & e)
            {
                if (input.size() > 0) {
                    auto token = input.front();
                    input.pop_front();
                    e.closing = token;
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

                brackets e; e.opening = token;

                if (token->text == "{") { e.body = bracketing(input, "}"); close(e); output += e; } else
                if (token->text == "(") { e.body = bracketing(input, ")"); close(e); output += e; } else
                if (token->text == "[") { e.body = bracketing(input, ")"); close(e); output += e; } else

                output += token;
            }
            return output;
        }
        array<element> bracketing (deque<token*> input) {
                return bracketing (input, "");
        }

        array<statement> statementing (array<element> input)
        {
            array<statement> output; statement s;

            if (log.messages.size() > 0) return output;
        
            for (auto && e : input)
            {
                if (std::holds_alternative<token*>(e) &&
                    std::get<token*>(e)->text == ";") {
                    if (s.elements.size() > 0 || s.body.size() > 0) {
                        output += s; s = statement{};
                    }
                }
                else
                if (std::holds_alternative<brackets>(e)) {
                    if (auto & b = std::get<brackets>(e);
                        b.opening->text == "{" &&
                        b.closing->text == "}") {
                        s.body = statementing(b.body);
                        if (s.body.size() == 0)
                            s.body += statement{};
                        output += s; s = statement{};
                    }
                }
                else s.elements += e;
            }

            if (s.elements.size() > 0 ||
                s.body.size() > 0)
                output += s;
    
            return output;
        }

        void combinaming (array<statement> & statements)
        {
            for (auto && s : statements)
            {
                array<element> elements; for (auto && e : s.elements)
                {
                    if (elements.size() > 0 &&
                        std::holds_alternative<name>(elements.back())) {
                        auto & names = std::get<name>(elements.back()).names;

                        if (std::holds_alternative<token*>(e))
                        {
                            token* t = std::get<token*>(e);

                            if (t->text == "::" &&
                                names.back().identifier != nullptr) {
                                names += unqualified_name{.coloncolon=t};
                                continue;
                            }
                            if (t->kind == "name" &&
                                names.back().coloncolon != nullptr &&
                                names.back().identifier == nullptr) {
                                names.back().identifier = t;
                                continue;
                            }
                        }

                        if (std::holds_alternative<brackets>(e)) {
                            names.back().params += std::get<brackets>(e);
                            continue;
                        }
                    }

                    if (std::holds_alternative<token*>(e) &&
                        std::get<token*>(e)->text == "::")
                        elements += name{array{unqualified_name{
                            .coloncolon=std::get<token*>(e)}}};
                    else
                    if (std::holds_alternative<token*>(e) &&
                        std::get<token*>(e)->kind == "name")
                        elements += name{array{unqualified_name{
                            .identifier=std::get<token*>(e)}}};
                    else
                        elements += e;
                }

                s.elements = elements;
                combinaming(s.body);
            }
        }

        array<statement> proceed (array<token> & input)
        {
            deque<token*> tokens;
            for (auto & token : input)
                if (token.kind != "space" &&
                    token.kind != "comment")
                    tokens += &token;

            auto statements = 
                statementing(
                bracketing(
                tokens));

            combinaming(statements);

            return statements;
        }
    };
}

