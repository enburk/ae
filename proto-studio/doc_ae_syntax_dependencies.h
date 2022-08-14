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

        array<path> inners;
        array<path> outers;

        void collect (statement& module, auto& cancel)
        {
            precedents.clear();

            visitor visitor;
            visitor.in_namepack = [this](namepack& n, statement*)
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
            visitor.pass(module, cancel);
        }

        void resolve (path path, report& log, auto& cancel)
        {
            dependees.clear();
            unresolved = precedents;
            for (token* t: precedents)
                t->kind = "module";

            auto dir = path;
            dir.replace_extension("");
            if (exists(dir) and
            is_directory(dir))
            resolve_in(cancel, dir); else
            resolve_in(cancel, path.parent_path());
            resolve_in(cancel, current_path()/"library");

            for (auto token: unresolved)
            log.error(token, "module "
                + bold(token->text) +
                " not found");

            // RIP dependency inversion
            auto parent = path.parent_path();
            parent.replace_extension(".ae");
            for (auto [name, p]: dependees) if (parent == p)
            for (auto t: precedents) if (t->text == name) {
            log.error(t, "circular dependency: <br>"
                "module " + bold(t->text) + " " +
                "resolved to parent module:"
                "<br>" + p.string());
                dependees.clear(); }

            if (true)
            for (auto [name, p]: dependees)
            log.debug("dependency: " + name
                + (char*)(u8" → ")
                + p.string());
        }

        void resolve_in (auto& cancel, path dir)
        {
            if (unresolved.empty()) return;

            using namespace std::filesystem;

            for (directory_iterator
            next(dir), end; next != end; ++next)
            {
                if (cancel) return;

                auto p = next->path();
                if (p.extension()==".ae"
                and is_regular_file(p)) // can throw
                {
                    str name = p.stem().string();
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
                resolve_in(cancel,
                dir.parent_path());
        }
    };
}

// |
// +- [ae]
// |    | 
// |    +-[library]
// |    |    |
// |    |    +- core.ae
// |    |    +-[core]
// |    |    |    |
// |    |    |    +- data.ae
// |    |    |    +- math.ae
// |    |    |
// |    |    +- net.ae
// |    |    +-[net]
// |    |    |    |
// |    |    |    +- url.ae
// |    |    |    +- tcp.ae
// |    |    |    +- http.ae
// |    |    |    +-[http]
// |    |    |    |    |
// |    |    |    |    +- redirect.ae
// |
// +-[repo]
// |    |
// |    +-[app]
// |    |    |
// |    |    +- app.ae!!
// |    |    +- subsystem.ae
// |    |    +-[subsystem]
// |    |    |    |
// |    |    |    + module1.ae
// |    |    |    + module2.ae
// |    |
// |    +-[libs]
// |    |    |
// |    |    +- lib1.ae
// |    |    +- lib2.ae
