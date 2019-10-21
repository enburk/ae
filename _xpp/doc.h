#pragma once
#include <queue>
#include "aux_array.h"
#include "aux_string.h"

struct Token
{
    str text, kind; int begin = 0;

    Token (              ) = default;
    Token (const Token & ) = default;
    Token (      Token &&) = default;
    Token (const str &  s) : text(s) {}
    Token (      str && s) : text(std::move(s)) {}
    Token & operator = (const Token & ) = default;
    Token & operator = (      Token &&) noexcept = default;
    Token & operator = (const str &  s) { *this = Token (s); return *this; }
    Token & operator = (      str && s) { *this = Token (std::move(s)); return *this; }

    friend array<Token> operator + (const Token  & a, const Token  & b) { array<Token> tt; tt += a; tt+= b; return tt; }
};

struct Entity { str kind, name; array<Token> head, tail; array<Entity> body; };

struct Document
{
    str type;

    Array<char>   text;
    Array<int>    lines;
    Array<Token>  lexica;
    Array<Token>  syntax;
    Array<Entity> code;

    void insert  (str);
    void insert  (str, int pos);
    void remove  (int pos, int n);
    void replace (int pos, int n, str);

    std::queue<Range<Token>> updates;

    struct Command { str kind; int pos, nn; str text; };
    array <Command> undo;
    array <Command> redo;
};



  