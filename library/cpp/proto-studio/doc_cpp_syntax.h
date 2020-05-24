#pragma once
#include "doc.h"
#include "doc_cpp_preprocessor.h"
namespace doc::syntax::cpp
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
            if (token->text != s) std::terminate();
            input.pop_front();
            e.tail += token;
        };

        array<element> output; while (input.size() > 0)
        {
            auto token = input.front();
            if (token->text == closing) break;
            input.pop_front();

            if (token->text == "}") { error(token, "unexpected '}'"); } else
            if (token->text == ")") { error(token, "unexpected ')'"); } else
            if (token->text == "]") { error(token, "unexpected ']'"); }

            element e; e.head += token;

            if (token->text == "{") { e.kind = "{}"; e.body = bracketing(input, "}"); close(e, "}"); } else
            if (token->text == "(") { e.kind = "()"; e.body = bracketing(input, ")"); close(e, ")"); } else
            if (token->text == "[") { e.kind = "[]"; e.body = bracketing(input, "]"); close(e, "]"); }

            output += e;
        }
        return output;
    }
    array<element> bracketing (deque<token*> input) {
            return bracketing (input, "");
    }
    /*
    array<Token> combinaming (array<Token> input)
    {
        array<Token> output; for (auto t : input)
        {
            t.tokens = combinaming (t.tokens);

            if (output.size () > 0)
            {
                auto & last = output.back ();

                if (last.type == "name::" && t.type == "name") { last.tokens += t; last.type = "name";   continue; }
                if (last.type == "name"   && t.text ==   "->") { last.tokens += t; last.type = "name::"; continue; }
                if (last.type == "name"   && t.text ==   "::") { last.tokens += t; last.type = "name::"; continue; }
                if (last.type == "name"   && t.text ==    ".") { last.tokens += t; last.type = "name::"; continue; }
                if (last.type == "name"   && t.type == u8"«»") { last.tokens += t; continue; }
                if (last.type == "name"   && t.type ==   "()") { last.tokens += t; continue; }
                if (last.type == "name"   && t.type ==   "[]") { last.tokens += t; continue; }
            }

            static std::set<string> keywords =
            {
                "struct", "class", "enum", "union", "namespace", "operator", "using",

                "inline", "noexcept", "const", "mutating", "static", "friend", "virtual", "explicit",

                "if", "then", "else", "for", "while", "do", "until", "return", "break", "continue"
            };

            if (keywords.find (t.text) != keywords.end ()) t.type = "keyword";

            if (t.text == "::") t.type = "name";

            output += t;
        }

        return output;
    }

    struct Name
    {
        array<Token> names;

        Name (Token token) { names += token; names [0].tokens.clear (); for (auto t : token.tokens) if (t.type == "name") names += t; else names.back ().tokens += t; }

        Token combine () const { Token token = names [0]; for (int i=1; i<names.size (); i++) token.tokens += names [i]; return token; }
    };
    */

    /*
    inline array<entity> parse (array<entity> input)
    {
        array<Entity> output; Entity entity; input += Token (";");
    
        for (auto token : input)
        {
            if (token.type == "{}") entity.body = parsing (token.tokens) + Entity () / * could be empty * /; else

            if (token.text != ";" && !token.text.headed ("#")) { entity.tokens += token; continue; }
            
            if (entity.tokens.size () > 0 || entity.body.size () > 0) output += entity; entity = Entity ();

            if (token.text.headed ("#")) { entity.tokens += token; entity.kind = "pragma"; output += entity; entity = Entity (); }
        }

        return output;
    }
    */
    inline array<element> parse (array<token> & input)
    {
        errors.clear();

        deque<token*> tokens;
        for (auto & token : input)
            if (token.kind != "space" &&
                token.kind != "comment")
                tokens += &token;

        return
            bracketing(
            preprocessor(
            tokens));
    }
}

