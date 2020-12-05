#pragma once
#include <map>
#include <cassert>
#include <compare>
#include <variant>
#include "aux_string.h"
#include "aux_utils.h"
#include "doc_text_layout.h"
namespace doc
{
    using text::token;

    struct entity
    {
        str name, kind, info;
        std::map<str,str> attr;
        array<token> head;
        array<entity> body;
        array<token> tail;
        array<str> debug;

        explicit entity() = default;
        explicit entity(str s) { head += token{s}; }
        explicit entity(array<token> tt) : head(tt) {}
        explicit entity(str name, str kind) : name(name), kind(kind) {}
    };

    struct report
    {
        struct message { token* token = nullptr; str kind, what; };
        array <message> messages;
        array <message> errors;

        void error (token* token, str what) {
            messages += message{token, "error", what};
            errors += message{token, "error", what};
            if (token) token->kind = "error";
        }

        void info (token* token, str what) {
            messages += message{token, "info", what};
        }

        str operator () () const
        {
            str s; for (auto [token, kind, what] : messages)
            {
                if (kind == "error") s += "<font color=#B00020>";

                if (token)
                    s += "("
                    + std::to_string(token->range.from.line+1) + ":"
                    + std::to_string(token->range.from.offset+1) + ") ";

                s += what;

                if (kind == "error") s += "</font>";

                s += "<br>";
            }
            return s;
        }
    };
}


  