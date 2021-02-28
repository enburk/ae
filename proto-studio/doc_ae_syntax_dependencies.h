#pragma once
#include <filesystem>
#include "doc_ae_syntax_visitor.h"
namespace doc::ae::syntax::dependencies
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    auto collect(array<statement> & statements) -> array<token*>
    {
        array<token*> dependencies;

        visitor visitor;
        visitor.on_namepack = [&dependencies](namepack& n)
        {
            if (n.names.size() > 0 and
                n.names[0].coloncolon and
                n.names[0].identifier) {

                token* t = n.names[0].identifier;

                if (dependencies.find_if([t](auto & d) {
                    return d->text == t->text; }) ==
                    dependencies.end())
                    dependencies += t;
            }
        };
        visitor.pass(statements);

        return dependencies;
    }

    auto all(path dir) -> array<path>
    {
        array<path> dependees;

        for (std::filesystem::recursive_directory_iterator
            next(dir), end; next != end; ++next)

            if (std::filesystem::is_regular_file (next->path()) &&
                next->path().extension().string() == ".ae")

                    dependees += next->path();

        return dependees;
    }

    auto resolve(path dir, array<token*> & dependencies, report & log) -> array<path>
    {
        array<path> dependees;
        array<token*> unresolved;

        if (not dir.has_relative_path()) {
            return resolve(std::filesystem::current_path() /
                "library", dependencies, log);
        }

        for (auto & dependency : dependencies)
        {
            bool found = false;

            for (std::filesystem::directory_iterator
                next(dir), end; next != end; ++next)
            {
                auto path = next->path();

                if (std::filesystem::is_regular_file(path) &&
                    path.filename().string() == dependency->text + ".ae") {
                    dependency->kind = "module";
                    dependees += path;
                    found = true;
                }

                if (std::filesystem::is_directory(path) &&
                    path.filename().string() == dependency->text) {
                    dependency->kind = "module";
                    dependees += all(path);
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

