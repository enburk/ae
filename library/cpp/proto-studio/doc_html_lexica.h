#pragma once
#include <map>
#include "doc_text_lexica.h"
namespace doc::lexica::html
{
    inline deque<token> parse (const text & text)
    {
        deque<token> output;
        
        str kind = "text";

        for (token t : doc::lexica::txt::parse(text))
        {
            if (output.size() > 0
            &&  output.back().kind == "text"
            &&  output.back().text.starts_with("&")
            && !output.back().text.ends_with(";")) {
                output.back().text += t.text;
                output.back().range.upto = t.range.upto;
                continue;
            }

            if (output.size() > 0
            &&  output.back().kind == "tag"
            &&  output.back().text.starts_with("\""))
            if (output.back().text.size() == 1
            || !output.back().text.ends_with("\"")) {
                output.back().text += t.text;
                output.back().range.upto = t.range.upto;
                continue;
            }

            if (output.size() > 0
            &&  output.back().kind == "tag"
            &&  output.back().text == "#") {
                output.back().text += t.text;
                output.back().range.upto = t.range.upto;
                continue;
            }

            if (t.text == "<") kind = "tag";

            t.kind = kind;  output += t;

            if (t.text == ">") kind = "text";
        }

        return output;
    }

    inline str encoded (str text)
    {
        text.replace_all("&", "&amp;"); // before anything else
        text.replace_all(" ", "&nbsp;");
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
 