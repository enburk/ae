#pragma once
#include "doc_ae_syntax.h"
namespace doc::ae
{
    struct entity
    {
        str name, kind, info;
        array<str> head;
        array<entity> body;
        array<str> tail;
        array<str> debug;

        void print (array<str> & lines, bool semicolon = true, int indent = 0)
        {
            str line;
            for (auto && s : head) line +=
                (line == "" or s == "," ? "" : " ")
                    + s;

            semicolon &= body.empty() and
                not line.starts_with("#");

            if (line != "" and semicolon) line += ";";
            if (line != "" and info !="") line += " ";
            if (info != "") line += "// " + info;
            if (line != "") line = str(' ', indent) + line;
            if (line != "") lines += line;

            if (not debug.empty())
                for (auto s : debug)
                    lines += str(' ', indent)
                        + "// " + s;

            if (not body.empty())
            {
                bool inner_semicolon = kind != "enum" && kind != "declarator";
                bool outer_semicolon = kind == "enum" or kind == "declarator"
                    or kind == "class" or kind == "function"
                    or kind == "union" or kind == "lambda";
            
                lines += str(' ', indent) + "{";

                    for (auto && e : body)
                        e.print (lines, inner_semicolon,
                            indent + 4);

                lines += str(' ', indent) + "}";
                
                if (outer_semicolon)
                    lines.back() += ";";
            }

            line = "";

            for (auto && s : tail) line +=
                (line == "" or s == "," ? "" : " ")
                    + s;
        
            if (line != "") lines += line;
        }
    };
}

