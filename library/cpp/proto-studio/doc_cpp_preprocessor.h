#pragma once
#include "doc.h"
namespace doc::cpp::syntax
{
    struct preprocessor
    {
        report & log;

        preprocessor (report & log) : log(log) {}

        deque<token*> proceed (deque<token*> & input)
        {
            deque<token*> output;

            struct define { array<str> args; array<token*> text; };

            std::unordered_map<str, define> defines;

            auto read = [this, &input](token*& token, str what)
            {
                if (input.size() == 0) log.error(token, what); else {
                    token = input.front();
                    input.pop_front();
                }
            };

            auto todo = [&input, read](token* token)
            {
                token->kind = "pragma";
                while (true) {
                    read(token, "unexpected end of file");
                    if (token->text == "\n") return; else
                        token->kind = "macros";
                }
            };

            auto parse = [this, &input, read, todo](token* token)
            {
                read(token, "preprocessor directive expected");
            
                str directive = token->text;

                if (directive == "if") todo(token); else
                if (directive == "elif") todo(token); else
                if (directive == "else") todo(token); else
                if (directive == "endif") todo(token); else
                if (directive == "ifdef") todo(token); else
                if (directive == "ifndef") todo(token); else
                if (directive == "define") todo(token); else
                if (directive == "undef") todo(token); else
                if (directive == "include") todo(token); else
                if (directive == "line") todo(token); else
                if (directive == "error") todo(token); else
                if (directive == "pragma") todo(token); else
                if (directive == "defined") todo(token); else

                log.error(token, "unrecognized preprocessing directive");
            };

            auto process = [&input, read, &output](token* t)
            {
                output += t;
            };

            bool newline = true;

            while (input.size() > 0)
            {
                auto token = input.front(); input.pop_front();

                if (token->text == "\n") newline = true; else

                if (!newline) process(token); else
                {
                    if (token->text == "#") {
                        token->kind = "pragma";
                        parse(token);
                    }
                    else {
                        newline = false;
                        process(token);
                    }
                }
            }
            return output;
        }
    };
}

