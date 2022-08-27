#pragma once
#include "doc_text_text.h"
namespace doc::ae::syntax
{
    using text::token;
    using text::report;

    struct expression;
    struct type
    {
        str name;
        str kind;
        array<type> genealogy;
    };

    struct brackets
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<expression> list;
    };

    struct nameunit
    {
        token* coloncolon = nullptr;
        token* identifier = nullptr;
        array<brackets> argss;
    };

    struct terminal { token* token = nullptr; type type; };

    struct namepack { array<nameunit> names; token* ellipsis = nullptr; type type; };

    struct operands { array<expression> list; };

    struct namelist { array<token*> list; };

    struct expression
    {
        std::variant
        <
            terminal,
            namepack,
            brackets,
            operands
        >
        variant;
        type
        type;
    };

    struct parameter
    {
        token* name = nullptr;
        namepack typexpr;
        expression value;
        type
        type;
    };

    struct parameters
    {
        token* opening = nullptr;
        token* closing = nullptr;
        array<parameter> list;
    };

    struct statement
    {
        str kind, variety;
        array<token*> names;
        parameters args;
        expression expr;
        namepack typexpr;
        array<statement> body;
        type type;

        statement* outer = nullptr;
        std::map<str, statement*> modules;
        std::multimap<str, statement*> members;

        str schema;
        str source;
    };

    str print (const brackets &);
    str print (const namepack &);
    str print (const expression &);
    str print (const parameters &);
    str print (const array<token*> & names)
    {
        str s;
        for (auto name: names) {
            s += name->text;
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
        if (b.opening) s += b.opening->text; s += es;
        if (b.closing) s += b.closing->text;
        return s;
    }

    str print (const parameters & pp)
    {
        str es;
        for (auto& p: pp.list) {
            es += print(p.typexpr);
            es += " ";
            es += p.name? p.name->text : "";
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

    str print (namepack const& namepack)
    {
        str s;
        for (auto name : namepack.names)
        {
            if (name.coloncolon) s += "::";
            if (name.identifier) s += 
                name.identifier->text;

            for (auto args : name.argss)
                s += print(args);
        }
        return s;
    }

    str print (expression const& e)
    {
        str s;

        std::visit(overloaded
        {
            [&](terminal v) { s += v.token->text + " "; },
            [&](namepack v) { s += print(v) + " "; },
            [&](brackets v) { s += print(v) + " "; },
            [&](operands v) {

                for (auto& o : v.list)
                s += print(o) + " ";
            },
        },
        e.variant);

        s.truncate();
        return s;
    }
}

