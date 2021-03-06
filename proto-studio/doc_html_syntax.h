﻿#pragma once
#include <deque>
#include "doc_html.h"
#include "doc_html_lexica.h"
namespace doc::html::syntax
{
    using text::token;

    inline deque<entity> parse (deque<token> tokens)
    {
        deque<entity> output;

        while (tokens.size() > 0)
        {
            token t = tokens.front(); tokens.pop_front();

            if (t.kind == "text")
            {
                if (output.size() == 0 ||
                    output.back().kind != "text")
                    output += entity {"", "text"};
            }
            else
            {
                if (output.size() == 0 || t.text == "<")
                    output += entity {"", "tag"};
            }

            output.back().head += t;
        }

        for (entity & e : output) if (e.kind == "tag")
        {
            tokens.clear();

            for (token t : e.head)
                if (t.text != " " && t.text != "\n" &&
                    t.text != "<" && t.text != ">")
                    tokens += t;

            if (tokens.size() == 0) {
                e.name = "error";
                continue;
            }
            if (tokens.front().text == "/")
            {
                e.name = tokens.size() == 2 ? tokens[1].text : "error";
                e.info = "closing";
                continue;
            }
            if (tokens.back().text == "/")
            {
                e.info = "closed";
                tokens.pop_back();
            }

            e.name = tokens.front().text.ascii_lowercased();
            tokens.pop_front();

            while (true)
            {
                if (tokens.size() == 0) break;
                str attr  = tokens.front().text; tokens.pop_front();
                if (tokens.size() == 0) { e.name = "error"; break; }
                str equal = tokens.front().text; tokens.pop_front();
                if (tokens.size() == 0) { e.name = "error"; break; }
                str value = tokens.front().text; tokens.pop_front();
                if (equal != "="      ) { e.name = "error"; break; }

                e.attr[attr] = value;
            }
        }
        else if (e.kind == "text")
        {
            for (token & t : e.head)
            {
                t.text.replace_all("&#x27;", "'");
                t.text.replace_all("&nbsp;", (char*)(u8"\u00A0"));
                t.text.replace_all("&amp;" , "&");
                t.text.replace_all("&lt;"  , "<");
                t.text.replace_all("&gt;"  , ">");
            }
        }

        return output;
    }

    inline array<entity> combine (deque<entity> & input, str closing)
    {
        array<entity> output;

        while (input.size() > 0)
        {
            entity e = input.front(); input.pop_front();

            if (e.kind == "text") output += e; else
            {
                if (e.info == "closing") {
                    if (e.name == closing) break; else
                        e.name = "error: /" + e.name;
                }

                if (e.name != "br" &&
                    e.info != "closed")
                    e.body += combine(input, e.name);

                output += e;
            }
        }

        return output;
    }
    inline array<entity> combine (deque<entity> input)
    {
        return combine(input, "");
    }
}
