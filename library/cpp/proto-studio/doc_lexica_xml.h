#pragma once
#include <map>
#include "doc_lexica_txt.h"
namespace doc::lexica::html
{
    inline deque<token> parse (const text & text)
    {
        deque<token> output;

        auto add = [&output, &text](token t)
        {
            if (t.text == "") return;
            if (t.kind == "text") output += t; else
            {
                int line = t.range.from.line;
                int from = t.range.from.offset;
                int upto = t.range.upto.offset;

                t = token {"", "tag", range {
                    place {line, from},
                    place {line, from}}};

                for (auto [c, offset] : text.lines[line].from(from).upto(upto))
                {
                    if (output.size() > 0
                    &&  output.back().kind == "tag"
                    &&  output.back().text.starts_with("\"")
                    && !output.back().text.ends_with("\"")) {
                        output.back() += c;
                        t.range.from.offset++;
                        t.range.upto.offset++;
                        output.back().range.upto = t.range.upto;
                        continue;
                    }

                    if (t.text.starts_with("\""))
                    {
                        t += c;
                        if (!t.text.ends_with("\"")) continue;
                        output += t;
                        t = token {"", "tag", range {
                            t.range.upto,
                            t.range.upto}};
                    }
                    else
                    if (c == "<" || c == "=" || c == "/" || c == ">")
                    {
                        if (t.text != "") output += t;
                        t = token {"", "tag", range {
                            t.range.upto,
                            t.range.upto}};

                        t += c;
                        output += t;
                        t = token {"", "tag", range {
                            t.range.upto,
                            t.range.upto}};
                    }
                    else
                    {
                        t += c;
                    }
                }

                if (t.text != "") output += t;
            }
        };

        str kind = "text";

        for (token t : doc::lexica::txt::parse(text))
        {
            if (t.text == " " ) { t.kind = kind; add(t); } else
            if (t.text == "\n") { t.kind = kind; add(t); } else
            {
                int line = t.range.from.line;
                int from = t.range.from.offset;
                int upto = t.range.upto.offset;

                t = token {"", kind, range {
                    place {line, from},
                    place {line, from}}};

                for (auto [c, offset] : text.lines[line].from(from).upto(upto))
                {
                    if (c == "<" && t.kind == "text")
                    {
                        add(t);
                        t = token {"", "tag", range {
                            t.range.upto,
                            t.range.upto}};
                        t += "<";
                    }
                    else
                    if (c == ">" && t.kind == "tag")
                    {
                        t += ">";
                        add(t);
                        t = token {"", "text", range {
                            t.range.upto,
                            t.range.upto}};
                    }
                    else
                    {
                        t += c;
                    }
                }

                kind = t.kind;

                add(t);
            }
        }

        return output;
    }

    inline str encoded (str text)
    {
        text.replace_all("&", "&amp;");
        text.replace_all("<", "&lt;");
        text.replace_all(">", "&gt;");
        return text;
    }

    inline str untagged (str & text)
    {
        str output;

        for (token t : parse(doc::text(text)))
        {
            if (t.text == "<br>") output += "\n"; else
            if (t.kind != "tag" ) output += t.text;
        }

        return output;
    }
}
 