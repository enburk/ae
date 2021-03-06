#pragma once
#include <atomic>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
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
        array<token> & input; report& log; std::atomic<bool>& stop; parser (
        array<token> & input, report& log, std::atomic<bool>& stop) :
        input(input), log(log), stop(stop)
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
                if (stop) break;

                auto token = input.front();

                if (token->text == "}" && closing == "}") break;
                if (token->text == ")" && closing == ")") break;
                if (token->text == "]" && closing == ")") break;
            
                input.pop_front();

                if (closing != "")
                if (token->kind == "space" or
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
                if (stop) break;
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
                if (stop) break;

                cluster cluster;

                for (auto && e : line)
                {
                    if (stop) break;
                    if (e.opening->kind != "space") cluster.elements += e;
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
                if (stop) break;

                array<cluster>* body = &output;

                while (true)
                {
                    if (stop) break;
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
                if (stop) break;

                cluster out{.tab = in.tab};

                for (auto && e : in.elements)
                {
                    if (stop) break;

                    if (e.opening->text == ";")
                    {
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
                output += std::move(out);
            }
    
            return output;
        }

        auto delimiting (array<element> input) -> array<element>
        {
            array<element> output; element o;

            for (auto && i : input)
            {
                if (stop) break;

                if (i.opening->text == ";")
                {
                    log.error(i.opening, "unexpected ;");
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
                else
                {
                    o.elements += std::move(i);
                }
            }

            if (not o.elements.empty())
                output += std::move(o);
    
            return output;
        }
    };

    str print (const element & e)
    {
        if (e.kind != "()") return
            e.opening ?
            e.opening->text :
            "";

        str s;
        if (e.opening) s += e.opening->text;
        
        for (auto & ee : e.elements) {
            s += print(ee);
            s += " ";
        }
        s.truncate();

        if (e.closing) s += e.closing->text;
        return s;
    }
}

