#pragma once
#include <span>
#include "doc_ae_syntax.h"
#include "doc_html_lexica.h"
namespace doc::ae::synthesis
{
    using namespace doc::ae::syntax;

    array<token> print (expression e);

    str print (brackets b)
    {
        str es;
        for (expression & e : b.list) {
            for (auto && t : print(e))
                es += t.text + " ";
            es.truncate();
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
    str print (str name)
    {
        if (name.contains("_")) return name;
        name = html::lexica::encoded(name);
        name.replace_all("&", "_");
        name.replace_all(";", "_");
        name.replace_all(".", "_");
        name.replace_all((char*)(u8"√"), "_sqrt_");
        if (name.starts_with("_") or name.contains("__")) name = "ae_" + name;
        return name;
    }
    str print (named_unit name)
    {
        str s;
        if (name.coloncolon) s += name.coloncolon->text;
        if (name.identifier) s += print(name.identifier->text);
        for (auto & p : name.arguments) s += print(p);
        return s;
    }
    str print (named_pack name)
    {
        str s; for (auto & unit : name.units) s += print(unit);
        return s;
    }

    array<token> print (expression e)
    {
        array<token> tokens;

        std::visit(aux::overloaded
        {
            [&](number  v) { tokens += token{v.token->text}; },
            [&](symbol  v) { tokens += token{v.token->text}; },
            [&](literal v) { tokens += token{str()

                + "std::span<uint8_t>((uint8_t*)(" + v.token->text + "), "
                    + std::to_string(v.token->text.size() - 2) + ")"};
            },
            [&](named_pack v) { tokens += token{print(v)}; },
            [&](operation  v) {

                if (v.title) tokens += token{print(v.title->text)};
                    
                if (v.title) tokens.back().text = "data::" + tokens.back().text;

                array<token> tt;
                for (auto o : v.operands)
                tt += print(o) + token{","};
                tt.truncate();

                tokens += token{"("} + tt + token{")"};
            },
            [&](brackets v)
            {
                tokens += token{print(v)};
            },
            [&](expression_if v)
            {
                tokens += token{"("};
                tokens += print(v.condition[0]);
                tokens += token{"?"};
                tokens += print(v.then_body[0]);
                tokens += token{":"};
                tokens += print(v.else_body[0]);
                tokens += token{")"};
            },
            [&](expression_for v)
            {
            },
            [&](lambda v)
            {
            },
        },
        e.variant);

        return tokens;
    }

    str print_type (named_pack name)
    {
        bool range = false;
        str type = print(name);
        if (type.ends_with("[]")) {
            type.truncate();
            type.truncate();
            range = true;
        }
        if (type == ""          ) type = "auto"; else
        if (type == "byte"      ) type = "uint8_t"; else
        if (type == "boolean"   ) type = "bool"; else
        if (type == "natural"   ) type = "unsigned"; else
        if (type == "natural.16") type = "uint16_t"; else
        if (type == "natural.32") type = "uint32_t"; else
        if (type == "natural.64") type = "uint64_t"; else
        if (type == "integer"   ) type = "int"; else
        if (type == "integer.16") type = "int16_t"; else
        if (type == "integer.32") type = "int32_t"; else
        if (type == "integer.64") type = "int64_t"; else
        if (type == "real"      ) type = "double"; else
        if (type == "real.32"   ) type = "float"; else
        if (type == "real.64"   ) type = "double"; else
        if (type == "real.80"   ) type = "long double"; else
        {}
        if (range) type = "std::span<"+type+">";
        return type;
    }
}

