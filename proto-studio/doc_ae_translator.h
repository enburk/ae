#pragma once
#include "doc_ae_synthesis_in.h"
#include "doc_ae_synthesis_out.h"
#include "doc_ae_synthesis_proc.h"
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::translator
{
    using synthesis::entity;
    using synthesis::statement;
    using std::filesystem::path;

    auto ext (path src, std::string ext){
        src.replace_extension(
        src.extension().string() + ext);
        return src; }

    auto hxx (path src){ return ext(src, ".h++"); }
    auto hpp (path src){ return ext(src, ".hpp"); }

    array<str> proceed (syntax::analysis::data& data, bool exe)
    {
        statement module = 
        synthesis::in(data.module);

        std::atomic<bool> cancel;
        synthesis::proceed(module, cancel);

        entity body;
        entity main;

        str 
        name = exe ? "ae" : data.name.text;
        name.replace_all(".", "_");
        body.head = "namespace " + name;
        main.head = "void main_()";

        for (auto path: data.dependencies.inners)
            body.body += entity{"#include \"" +
            hpp(path).string() + "\""};

        for (auto& statement: module.body)
            synthesis::out(statement,
            statement.kind == "type" or
            statement.kind == "alias" or
            statement.kind == "variable" or
            statement.kind == "singleton" or
            statement.kind == "function" or
            statement.kind == "operator"?
            body.body:
            main.body);

        if (exists(hxx(data.path)))
        body.body += entity{"#include \"" +
        hxx(data.path).string() + "\""};

        if (main.body.empty())
            main.body +=
            entity{};

        body.body += main;

        array<str> lines;
        body.print(lines);
        return lines;
    }
}

