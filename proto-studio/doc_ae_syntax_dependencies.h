#pragma once
#include <filesystem>
#include "doc_ae_syntax_visitor.h"
namespace doc::ae::syntax
{
    using namespace std::filesystem;

    // if A depends on B
    // == B is depended on by A
    // => A is the depender
    // => B is the dependee

    struct dependencies
    {
        array<token*> precedents;
        array<token*> unresolved;
        std::map<str, path>
            dependees;

        void collect (array<statement>& statements)
        {
            precedents.clear();

            visitor visitor;
            visitor.on_namepack = [this](namepack& n)
            {
                if (n.names.size() > 0
                and n.names.front().coloncolon
                and n.names.front().identifier)
                {
                    token*
                    t = n.names.front().identifier;
                    t->kind = "module";

                    if (precedents.find_if([t](auto & d) {
                        return d->text == t->text; }) ==
                        precedents.end())
                        precedents += t;
                }
            };
            visitor.pass(statements);
        }

        void resolve (path p, report& log)
        {
            dependees.clear();
            unresolved = precedents;

            auto dir = p;
            dir.replace_extension("");
            if (exists(dir) and
            is_directory(dir))
            resolve_in(dir); else
            resolve_in(p.parent_path());
            resolve_in(current_path()/"library");

            for (auto token: unresolved)
            log.error(token, "module ::"
                + token->text +
                " not found");

            if (true)
            for (auto [name, p]: dependees)
            log.debug("dependency: " + name
                + (char*)(u8" → ")
                + p.string());
        }

        void resolve_in (path dir)
        {
            if (unresolved.empty()) return;

            using namespace std::filesystem;

            for (directory_iterator
            next(dir), end; next != end; ++next)
            {
                auto p = next->path();
                if (is_regular_file(p)
                and p.extension()==".ae")
                {
                    str name = p.filename().string();
                    auto it = unresolved.find_if([name]
                    (auto t){ return t->text == name; });
                    if (it == unresolved.end()) continue;
                    dependees.emplace(name, p);
                    unresolved.erase(it);
                    if (unresolved.empty())
                        return;
                }
            }

            if (dir.has_relative_path())
            resolve_in(dir.parent_path());
        }
    };
}

