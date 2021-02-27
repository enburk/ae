#pragma once
#include <filesystem>
#include "doc_ae_syntax_visitor.h"
namespace doc::ae::syntax::dependencies
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    auto collect(array<statement> & statements) -> array<str>
    {
        array<str> dependencies;

        visitor visitor;
        visitor.on_name = [&dependencies](namepack& n)
        {
            if (n.names.size() > 0 and
                n.names[0].coloncolon) dependencies.try_emplace(
                n.names[0].identifier->text);
        };
        visitor.pass(statements);

        return dependencies;
    }

    auto resolve(path p, array<str> & dependencies, report & log) -> array<path>
    {
        array<path> dependees;

        //for (auto & dependency : dependencies)
        {
            //if (not std::filesystem::exists(dependee))
        }

        return dependees;
    }
}

