#pragma once
#include "doc_text_text.h"
#include "doc_ae_syntax.h"
namespace doc::ae
{
    using namespace syntax;

    str print (const str &);
    str print (const brackets &);
    str print (const namepack &);
    str print (const expression &);
    str print (const parameters &);

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
        if (b.opening) s += b.opening->text; s += es;
        if (b.closing) s += b.closing->text;
        return s;
    }

    str print (const namepack & namepack)
    {
        str s;
        for (auto name : namepack.names)
        {
            if (name.coloncolon) s += "::";
            if (name.identifier) s += print(name.identifier->text);
            for (auto arg : name.args) s += print(arg);
        }
        return s;
    }

    str print (const str & name)
    {
        static std::map<doc::text::glyph, str> symbols;
        
        if (symbols.empty())
        {
            for (auto [s, c] : doc::html::lexica::symbols)
            {
                auto glyphs = doc::text::glyph::parse((char*)(c));

                if (glyphs.size() == 1) symbols.emplace(glyphs.front(), s);
            }
        }

        str s;

        for (auto glyph : doc::text::glyph::parse(name))
        {
            if (not glyph.letter() and
                not glyph.digit())
            {
                if (auto
                    it = symbols.find(glyph);
                    it != symbols.end())
                {
                    s += "_" + it->second + "_";
                    continue;
                }
            }
            s += glyph;
        }

        if (s.starts_with("_") or
            s.starts_with("ae_"))
            s = "ae_" + s;

        s.replace_all("__", "_");
        return s;
    }

    str print (const expression & e)
    {
        str s;

        std::visit(overloaded
        {
            [&](terminal v) { s += " " + print(v.token->text); },
            [&](namepack v) { s += " " + print(v); },
            [&](brackets v) { s += " " + print(v); },
            [&](operands v) {

                if (v.list.empty()) return;
                for (auto & o : v.list)
                s += print(o) + " ";
                s.truncate();
            },
        },
        e.variant);

        return s;
    }
}

