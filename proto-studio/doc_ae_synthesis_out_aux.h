#pragma once
#include "doc_html_lexica.h"
#include "doc_ae_synthesis.h"
namespace doc::ae::synthesis
{
    str print (const str &);
    str print (const brackets &);
    str print (const namepack &);
    str print (const expression &);
    str print (const parameters &);
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

    str print (const parameters & pp)
    {
        str es;
        for (auto& p: pp.list) {
            es += print(p.type.name);
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

    str print (const namepack & namepack)
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
        return s;
    }

    str print (expression const& e)
    {
        str s;

        std::visit(overloaded
        {
            [&](terminal v) { s += " " + print(v.text); },
            [&](namepack v) { s += " " + print(v); },
            [&](brackets v) { s += " " + print(v); },
            [&](operands v) {

                for (auto& o: v.list)
                s += print(o) + " ";
                s.truncate();
            },
        },
        e.variant);

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
