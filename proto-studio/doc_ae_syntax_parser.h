#pragma once
#include "doc_text_text.h"
namespace doc::ae::syntax
{
    using text::token;
    using text::report;

    struct element
    {
        str kind;
        token* opening = nullptr;
        token* closing = nullptr;
        array<element> elements;
    };

    struct cluster
    {
        int tab = 0;
        array<element> elements;
        array<cluster> clusters;
    };

    struct parser
    {
        array<cluster> output;
        array<token> & input; report & log; parser (
        array<token> & input, report & log) : input(input), log(log)
        {
            deque<token*> tokens;
            for (auto & token : input)
                if (token.kind != "comment")
                    tokens += &token;

            output = 
            delimiting(
            clustering(
            tabulating(
            scanlining(
            bracketing(
            tokens)))));
        }

        auto bracketing (deque<token*> & input, str closing = "") -> array<element>
        {
            auto close = [this, &input](element & e)
            {
                if (input.size() > 0) {
                    auto token = input.front();
                    input.pop_front();
                    e.closing = token;
                    e.kind = "()";
                }
                else log.error(
                    e.opening, "unclosed '" +
                    e.opening->text + "'");
            };

            array<element> output; while (input.size() > 0)
            {
                auto token = input.front();

                if (token->text == "}" && closing == "}") break;
                if (token->text == ")" && closing == ")") break;
                if (token->text == "]" && closing == ")") break;
            
                input.pop_front();

                if (token->kind == "space" &&
                    token->text == "\n")
                    continue;

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

        auto scanlining (array<element> input) -> array<array<element>>
        {
            array<array<element>> output; array<element> line;

            for (auto & e : input)
            {
                if (e.opening->text != "\n") line += e; else
                if (line.size() > 0) output += std::move(line);
            }
            if (line.size() > 0) output += std::move(line);

            return output;
        }

        auto tabulating (array<array<element>> input) -> array<cluster>
        {
            array<cluster> output;

            for (auto && line : input)
            {
                cluster cluster;

                for (auto && e : line)
                {
                    if (e.opening->text != " ") cluster.elements += e;
                    else if (cluster.elements.size() == 0)
                        cluster.tab++;
                }
                if (cluster.elements.size() > 0)
                    output += cluster;
            }

            return output;
        }

        auto clustering (array<cluster> input) -> array<cluster>
        {
            array<cluster> output;

            for (auto && line : input)
            {
                array<cluster>* body = &output;

                while (true)
                {
                    if (body->rbegin() != body->rend() &&
                        body->rbegin()->tab < line.tab) { body = &
                        body->rbegin()->clusters;
                        continue;
                    }
                    body->push_back(line);
                    break;
                }
            }

            return output;
        }

        auto delimiting (array<cluster> input) -> array<cluster>
        {
            array<cluster> output;

            for (auto && in : input)
            {
                cluster out{.tab = in.tab};

                for (auto && e : in.elements)
                {
                    if (e.opening->text == ";")
                    {
                        if (out.elements.empty()) continue;
                        output += std::move(out);
                        out = cluster{.tab = in.tab};
                    }
                    else
                    if (e.kind == "()")
                    {
                        e.elements = delimiting(e.elements);
                        out.elements += std::move(e);
                    }
                    else out.elements += std::move(e);
                }

                out.clusters = delimiting(in.clusters);
            }
    
            return output;
        }

        auto delimiting (array<element> input) -> array<element>
        {
            array<element> output; element o;

            for (auto && i : input)
            {
                if (i.opening->text == ";")
                {
                    log.error(i.opening, "unexpected ';'");
                }
                if (i.opening->text == ",")
                {
                    if (o.elements.empty()) continue;
                    output += std::move(o);
                    o = element{};
                }
                else
                if (i.kind == "()")
                {
                    i.elements = delimiting(i.elements);
                    o.elements += std::move(i);
                }
                else o.elements += i;
            }

            if (not o.elements.empty())
                output += std::move(o);
    
            return output;
        }
    };
}

