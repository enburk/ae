﻿#pragma once
#include "doc_html_lexica.h"
#include "doc_html_syntax.h"
namespace doc::html
{
    inline str encoded (str text)
    {
        text.replace_all("&", "&amp;"); // before anything else
        text.replace_all("<", "&lt;");
        text.replace_all(">", "&gt;");
        text.replace_all("\n", "<br>");
        return text;
    }

    inline str untagged (const str & text)
    {
        str output;

        for (text::token t : lexica::parse(text::text(text)))
        {
            if (t.text == "<br>") output += "\n"; else
            if (t.kind != "tag" ) output += t.text;
        }

        return output;
    }

    inline array<entity> entities (const str & text)
    {
        return
        syntax::combine(
        syntax::parse(
        lexica::parse(
        text::text(text))));
    }

    inline void print (const entity & e, array<text::token> & output, int indent = 0)
    {
        using text::token;

        output += token{"\n"};
        output += token{str(' ', indent)};

        if (e.kind == "text")
        {
            for (token t : e.head)
            {
                if (t.text == "\n") t.text = " ";
                if (t.text == " " &&
                    output.size() > 0 &&
                    output.back().text == " ") continue;

                output += t;
            }
        }
        else
        {
            output += token{"<"};
            if (e.info == "closing") output += token{"/"};

            output += token{e.name};
            for (auto [attr, value] : e.attr)
                output += token{" " + attr + "=" + value};

            if (e.info == "closed") output += token{"/"};
            output += token{">"};

            for (const entity & nested : e.body)
                print (nested, output, indent+4);
        }
    }
    inline array<text::token> print (const array<entity> & entities)
    {
        array<text::token> output;

        for (auto & e : entities) print (e, output);

        return output;
    }
}
