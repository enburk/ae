#pragma once
#include <queue>
#include "aux_array.h"
#include "aux_string.h"
namespace doc
{
    struct Token
    {
        str text, kind; int offset = 0;

        Token (              ) = default;
        Token (const Token & ) = default;
        Token (      Token &&) = default;
        Token (const str &  s) : text(s) {}
        Token (      str && s) : text(std::move(s)) {}
        Token & operator = (const Token & ) = default;
        Token & operator = (      Token &&) noexcept = default;
        Token & operator = (const str &  s) { *this = Token (s); return *this; }
        Token & operator = (      str && s) { *this = Token (std::move(s)); return *this; }

        friend array<Token> operator + (Token a, Token b) { array<Token> tt; tt += a; tt+= b; return tt; }

        friend bool operator == (const Token & a, const Token & b) { return !(a != b); }
        friend bool operator != (const Token & a, const Token & b) { return
            a.text   != b.text ||
            a.kind   != b.kind ||
            a.offset != b.offset;
        }
    };

    struct Entity { str kind, name; array<Token> head, tail; array<Entity> body; };

    struct Document
    {
        str type = "text";

        struct line { int offset = 0; bool viewed = false; bool saved = false; };

        Array<char>   text;
        Array<line>   lines;
        Array<Token>  lexica;
        Array<Token>  syntax;
        Array<Entity> code;

        struct Command { str kind; int pos, nn; str text; };
        array <Command> undos; void undo ();
        array <Command> redos; void redo ();
        void perform (Command, int dir = 1);

        void upload  (str s)            { perform(Command{"insert", text.size(), 0, std::move(s)}, 0); } // w/o undo
        void append  (str s)            { perform(Command{"insert", text.size(), 0, std::move(s)}); }
        void insert  (int pos, str s)         { perform(Command{"insert",  pos,  0, std::move(s)}); }
        void replace (int pos, int nn, str s) { perform(Command{"replace", pos, nn, std::move(s)}); }
        void remove  (int pos, int nn)        { perform(Command{"remove",  pos, nn, ""}); }

        std::queue<Range<Token>> updates;
    };
}


  