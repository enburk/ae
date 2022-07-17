#pragma once
#include "doc_html_lexica.h"
#include "doc_ae_synthesis.h"
namespace doc::ae::synthesis
{
    str print (const str &);
    str print (const expression &);
    str print (const namelist & names)
    {
        str s;
        for (auto name: names.list) {
            s += print(name);
            s += ",";
            s += " ";
        }
        s.truncate();
        s.truncate();
        return s;
    }

    str print (const brackets & b)
    {
        str es;
        for (auto & e : b.list) {
            es += print(e);
            es += ",";
            es += " ";
        }
        es.truncate();
        es.truncate();

        str s;
        s += b.opening; s += es;
        s += b.closing;
        return s;
    }

    str print (const namepack & namepack, bool type = false)
    {
        str s;
        for (auto name : namepack.names)
        {
            s += print(name.identifier);
            for (auto args : name.argss)
            s += print(args);
            s += "::";
        }
        s.truncate();
        s.truncate();


        if (s == "type") s = "auto";
        if (s == "byte[]") s = "std::span<uint8_t>";

        return s;
    }

    str print (const parameters & pp)
    {
        str es;
        for (auto& p: pp.list) {
            es += print(p.typexpr);
            es += " ";
            es += print(p.name);
        //  es += "=";
        //  es += print(p.value);
            es += ",";
            es += " ";
        }
        es.truncate();
        es.truncate();

        str s;
        s += "("; s += es;
        s += ")";
        return s;
    }

    str print (expression const& e)
    {
        str s;

        std::visit(overloaded
        {
            [&](terminal v)
            {
                if (v.kind == "literal")
                s += "std::span((uint8_t*)" + v.text + "," +
                std::to_string(v.text.size()) + ")"; else
                s += v.text;
                s += " ";
            },
            [&](namepack v) { s += print(v) + " "; },
            [&](brackets v) { s += print(v) + " "; },
            [&](operands v)
            {
                for (auto& o: v.list)
                s += print(o) + " ";
            },
        },
        e.variant);

        s.truncate();
        return s;
    }

    str print (const str & name)
    {
        static std::map<doc::text::glyph, str> symbols;
        
        if (symbols.empty())
        {
            for (auto [s, c] : doc::html::lexica::symbols)
            {
                auto glyphs = aux::unicode::array((char*)(c));

                if (glyphs.size() == 1) symbols.emplace(
                    glyphs.front(), s);
            }
        }

        str s;

        for (auto glyph: aux::unicode::glyphs(name))
        {
            if (not glyph.letter() and
                not glyph.digit())
            {
                if (auto
                it = symbols.find(glyph);
                it != symbols.end()) {
                s += "_" + it->second + "_";
                continue; }
            }
            s += glyph;
        }

        if (s.starts_with("_") or
            s.starts_with("ae_"))
            s = "ae_" + s;

        s.replace_all("__", "_");
        return s;
    }
}
