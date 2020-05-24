#pragma once
#include <deque>
#include "doc_html_lexica.h"
namespace doc::syntax::html
{
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
                t.text.replace_all("&nbsp;", u8"\u00A0");
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
                if (e.info == "closing")
                    if (e.name == closing) break;
                    else e.name = "error: /" + e.name;

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

    inline void print (const entity & e, array<token> & output, int indent = 0)
    {
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
    inline array<token> print (const array<entity> & entities)
    {
        array<token> output;

        for (auto & e : entities) print (e, output);

        return output;
    }
}
