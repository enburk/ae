#pragma once
#include <filesystem>
#include "doc_ae_syntax_visitor.h"
namespace doc::ae::syntax::dependencies
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    auto collect(array<statement> & statements)
    {
        array<token*> dependencies;

        visitor visitor;
        visitor.on_namepack = [&dependencies](namepack& n)
        {
            if (n.names.size() > 0 and
                n.names[0].coloncolon and
                n.names[0].identifier) {

                token* t = n.names[0].identifier;
                t->kind = "module";

                if (dependencies.find_if([t](auto & d) {
                    return d->text == t->text; }) ==
                    dependencies.end())
                    dependencies += t;
            }
        };
        visitor.pass(statements);

        return dependencies;
    }

    auto all(path dir)
    {
        array<path> dependees;

        for (std::filesystem::recursive_directory_iterator
            next(dir), end; next != end; ++next)

            if (std::filesystem::is_regular_file (next->path()) &&
                next->path().extension().string() == ".ae")

                    dependees += next->path();

        return dependees;
    }

    array<std::pair<str, path>>
    resolve(path dir, array<token*> & dependencies, report & log)
    {
        array<std::pair<str, path>> dependees;
        array<token*> unresolved;

        if (not dir.has_relative_path()) {
            return resolve(std::filesystem::current_path() /
                "library", dependencies, log);
        }

        for (auto & dependency : dependencies)
        {
            str name = dependency->text; bool found = false;

            for (std::filesystem::directory_iterator
                next(dir), end; next != end; ++next)
            {
                auto path = next->path();

                if (std::filesystem::is_regular_file(path) &&
                    path.filename().string() == name + ".ae") {
                    dependees += std::pair{name, path};
                    found = true;
                }

                if (std::filesystem::is_directory(path) &&
                    path.filename().string() == name) for (auto p : all(path)) {
                    dependees += std::pair{name, p};
                    found = true;
                }
            }

            if (not found) unresolved += dependency;
        }

        if (not unresolved.empty())
        {
            if (dir == std::filesystem::current_path() / "library")
                for (auto token : unresolved)
                    log.error(token, "module ::"
                        + token->text + " not found");

            else dependees += resolve(
                dir.parent_path(),
                unresolved, log);
        }

        return dependees;
    }
}

