#pragma once
#include "doc_text_text.h"
namespace doc::ae::synthesis
{
    struct entity
    {
        str head, kind, info;
        array<entity> body;
        array<str> infos;

        void print (array<str>& lines, bool semicolon = true, int indent = 0)
        {
            if (kind == "empty") return;

            str line = head;

            semicolon &= body.empty() and
                not line.starts_with("#");

            if (line != "" and semicolon) line += ";";
            if (line != "" and info !="") line += " ";
            if (info != "") line += "// " + info;
            if (line != "") line = str(' ', indent) + line;
            if (line != "") lines += line;

            if (not infos.empty())
                for (auto s : infos)
                    lines += str(' ', indent)
                        + "// " + s;

            if (not body.empty())
            {
                bool inner_semicolon = kind != "enum" && kind != "declarator";
                bool outer_semicolon = kind == "enum" or kind == "declarator"
                    or kind == "class" or kind == "function"
                    or kind == "union" or kind == "lambda";

                int n0 = lines.size();
                lines += str(' ', indent) + "{";

                    for (auto && e : body)
                        e.print (lines, inner_semicolon,
                            indent + 4);

                lines += str(' ', indent) + "}";
                int n1 = lines.size();
                
                if (outer_semicolon)
                    lines.back() += ";";

                if (n1-n0 == 3 and n0 > 0
                    and not lines[n1-4].contains("//")
                    and not lines[n1-2].contains("//")) {
                    lines[n1-3].strip(); lines[n1-4] += " " + lines[n1-3];
                    lines[n1-2].strip(); lines[n1-4] += " " + lines[n1-2];
                    lines[n1-1].strip(); lines[n1-4] += " " + lines[n1-1];
                    lines.resize(n0);
                }
                else
                if (n1-n0 == 2 and n0 > 0
                    and not lines[n1-3].contains("//")) {
                    lines[n1-2].strip(); lines[n1-3] += " " + lines[n1-2];
                    lines[n1-1].strip(); lines[n1-3] += " " + lines[n1-1];
                    lines.resize(n0);
                }
            }
        }
    };
}

