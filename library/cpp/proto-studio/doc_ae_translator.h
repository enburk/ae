#pragma once
#include <filesystem>
#include "doc_ae_lexica.h"
#include "doc_ae_syntax_analysis.h"
#include "doc_ae_synthesis.h"
namespace doc::ae::translator
{
    report log;

    using syntax::element;
    using syntax::scope;

    scope global_scope;

    str load (std::filesystem::path path)
    {
        std::ifstream stream(path); str text = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};

        if (text.starts_with("\xEF" "\xBB" "\xBF"))
            text.upto(3).erase(); // UTF-8 BOM

        return text;
    }

    void print (entity e, array<str> & lines, int indent = 0, bool semicolon = true)
    {
        str line;

        //for (auto && e : s.elements)
        //    if (std::holds_alternative<str>(e))
        //        line += std::get<str>(e) + " ";
    
        line.truncate ();

        if (e.info != "") line += " // " + e.info;

        semicolon &= !line.starts_with("#") && e.body.size() == 0;

        if (line != "" && semicolon) line += ";";

        if (line != "") line = str(' ', indent) + line;
        
        if (line != "") lines += line;

        if (e.body.size() > 0)
        {
            bool inner_semicolon = e.kind != "enum" && e.kind != "declarator";
            bool outer_semicolon = e.kind == "enum" || e.kind == "declarator"
                || e.kind == "function" || e.kind == "class"
                || e.kind == "lambda"   || e.kind == "union";
            
            lines += str(' ', indent) + "{";

                for (auto & e : e.body) print (e, lines, indent + 4, inner_semicolon);

            lines += str(' ', indent) + "}"; if (outer_semicolon) lines.back() += ";";
        }
    }


    void add (std::filesystem::path path)//, syntax::scope & scope = global_scope)
    {
    }

    void run (std::filesystem::path path)//, syntax::scope & scope = global_scope)
    {
        array<token> tokens = lexica::parse(text{load(path)});
        array<entity> statements = 
            synthesis::proceed(
            syntax::analysis(log).proceed(
                tokens));

        array<str> lines;
        for (auto && s : statements) print (s, lines);

        path.replace_extension(path.extension().string() + ".cpp");
        {
            std::ofstream result(path);
            result << "\xEF" "\xBB" "\xBF"; // UTF-8 BOM
            for  (auto && line :lines) {
                result << line;
                result << "\n";
            }
        }

        std::filesystem::path cl = "c:\\Program Files (x86)"
            "\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC"
            "\\14.26.28801\\bin\\Hostx64\\x64\\cl.exe";

        if (!std::filesystem::exists(cl)) {
            log.error(nullptr, "C++ compiler not found");
            return;
        }

        std::filesystem::path cllog = "cl.log.txt";
        if (std::filesystem::exists(cllog)) 
            std::filesystem::remove(cllog);
        try
        {
            sys::process compile(cl, "\"" + path.string() + "\"",
            sys::process::options{.hidden = true, .out = cllog});
            if (!compile.wait(5*1000)) throw std::runtime_error(
                "It takes too long.");
        }
        catch (const std::exception & e) {
            log.error(nullptr, e.what());
            return;
        }

        if (str sss = load(cllog);
            sss.find("error:") or sss.find("warning:") or
            sss.find("error ") or sss.find("warning "))
        {
            for (str s : sss.split())
            log.error(nullptr, doc::html::lexica::encoded(s));
            return;
        }
    }
}

