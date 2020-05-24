#pragma once
#include "doc.h"
namespace doc::syntax::ae
{
    array<element> bracketing (deque<token*> & input, str closing)
    {
        auto close = [&input](element & e, str s)
        {
            if (input.size() == 0) {
                error(e.head[0], "unclosed '" + e.head[0]->text + "'");
                return;
            }
            auto token = input.front();
            input.pop_front();
            e.tail += token;
        };

        array<element> output; while (input.size() > 0)
        {
            auto token = input.front();

            if (token->text == "}" && closing == "}") break;
            if (token->text == ")" && closing == ")") break;
            if (token->text == "]" && closing == ")") break;
            
            input.pop_front();

            if (token->text == "}") { error(token, "unexpected '}'"); } else
            if (token->text == ")") { error(token, "unexpected ')'"); } else
            if (token->text == "]") { error(token, "unexpected ']'"); }

            element e; e.head += token;

            if (token->text == "{") { e.kind = "{}"; e.body = bracketing(input, "}"); close(e, "}"); } else
            if (token->text == "(") { e.kind = "()"; e.body = bracketing(input, ")"); close(e, ")"); } else
            if (token->text == "[") { e.kind = "()"; e.body = bracketing(input, ")"); close(e, ")"); }

            output += e;
        }
        return output;
    }
    array<element> bracketing (deque<token*> input) {
            return bracketing (input, "");
    }

    inline array<element> parse (array<token> & input)
    {
        errors.clear();

        deque<token*> tokens;
        for (auto & token : input)
            if (token.kind != "space" &&
                token.kind != "comment")
                tokens += &token;

        return bracketing(tokens);
    }
}

