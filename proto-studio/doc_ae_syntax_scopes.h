#pragma once
#include <map>
#include <list>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    void scope (statement& s, report& log, auto& cancel)
    {
        std::map<str, str> names;

        for (auto& arg: s.args.list)
            if (names.contains(arg.name->text))
                log.error(arg.name, "name " +
                    bold(arg.name->text) +
                    " already exists");
            else
            names.emplace(
            arg.name->text,
                "arg");
        
        for (auto& statement: s.body)
        {
            statement.outer = &s;

            if (statement.kind == "precedence") continue;

            for (token* name: statement.names)
            {
                s.members.emplace(name->text, &statement);

                auto i = names.find(name->text);
                if (i == names.end()) {
                    names.emplace(name->text, statement.kind);
                    continue; }

                if (statement.kind != i->second) {
                    log.error(name, "already exists " +
                        i->second + " " +
                        bold(name->text));
                        break; }

                if (statement.kind != "type"
                and statement.kind != "function"
                and statement.kind != "operator") {
                    log.error(name, "already exists " +
                        i->second + " " +
                        bold(name->text));
                        break; }
            }
        }
    }
}
