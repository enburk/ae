#pragma once
#include "cpp_data.h"

    inline constexpr auto ascii (char c) { return c >= ' ' && c <= '~'; };
    inline constexpr auto space (char c) { return c == ' ' || c =='\t'; };
    inline constexpr auto digit (char c) { return c >= '0' && c <= '9'; };
    inline constexpr auto alpha (char c) { return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z'; };

deque<Token> lexica (array<Source> & sources, string file)
{
    deque<Token> output; Source & source = sources.back (); source.lines += ""; Token t;

    bool UTF_8_BOM = false; if (file.headed("\xEF" "\xBB" "\xBF")) { UTF_8_BOM = true; file.erase (0,3); }

    file += "\n\n"; // happy ending

    for (char c : file)
    {
        if (c != '\n' && !ascii(c) && !space(c) && !UTF_8_BOM) throw Error ("Lexica: non-ASCII character without UTF-8 BOM", t);

        if (c == '\n') source.lines += ""; else source.lines.back () += c;

        if (t.text == "/*"  && c != '*') {                 continue; }
        if (t.text == "/*"  && c == '*') { t.text = "/**"; continue; }
        if (t.text == "/**" && c != '/') { t.text = "/*";  continue; }
        if (t.text == "/**" && c == '/') { t.text = "";    continue; }

        if (t.text == "//"  && c !='\n') {                 continue; }
        if (t.text == "//"  && c =='\n') { t.text = "";    continue; }

        bool same = false;

        if (t.type == "space"  ) same = space (c); else
        if (t.type == "name"   ) same = alpha (c) || digit (c) || c == '_'; else
        if (t.type == "number" ) same = alpha (c) || digit (c) || c == '.'; else
        if (t.type == "literal") same = t.text == "\"" || !t.text.tailed ("\"") || t.text.tailed ("\\\""); else
        if (t.type == "char"   ) same = t.text == "\'" || !t.text.tailed ("\'") || t.text.tailed ("\\\'"); else
        if (t.type == "symbol" ) same = false
                
        ||  t.text == "+" && ( c == '=' || c == '+' )
        ||  t.text == "-" && ( c == '=' || c == '-' || c == '>' )
        ||  t.text == "/" && ( c == '=' || c == '*' || c == '/' )
        ||  t.text == "*" && ( c == '=' )

        ||  t.text == "<" && ( c == '=' || c == '<' )
        ||  t.text == ">" && ( c == '=' || c == '>' )
        ||  t.text == "&" && ( c == '=' || c == '&' )
        ||  t.text == "|" && ( c == '=' || c == '|' )
        ||  t.text == "!" && ( c == '=' )
        ||  t.text == "=" && ( c == '=' )
        ||  t.text == ":" && ( c == ':' )

        ||  t.text =="<<" && ( c == '=' )
        ||  t.text ==">>" && ( c == '=' )

        ||  t.text == "." && ( c == '.' )
        ||  t.text ==".." && ( c == '.' )
        
        ||  t.text.size () == 1 && (static_cast<uint8_t>(t.text [0]) & 0b11000000) == 0b11000000 // UTF-8: 110xxxxx 10xxxxxx
        ||  t.text.size () == 2 && (static_cast<uint8_t>(t.text [0]) & 0b11100000) == 0b11100000 // UTF-8: 1110xxxx 10xxxxxx 10xxxxxx
        ||  t.text.size () == 3 && (static_cast<uint8_t>(t.text [0]) & 0b11110000) == 0b11110000 // UTF-8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        ;
        if (same) t.text += c; else
        {
            if (t.text != "") output += t;

            t.text   = c;
            t.source = sources.             size () - 1;
            t.line   = source.lines.        size () - 1;
            t.column = source.lines.back ().size () - 1;
            t.type   =

                alpha (c) ? "name"    :
                digit (c) ? "number"  :
                space (c) ? "space"   : 
                c == '\"' ? "literal" : 
                c == '\'' ? "char"    : 
                            "symbol"  ;
        }
    }

    return output;
}
