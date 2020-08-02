#pragma once
#include <filesystem>
#include "../sys.h"
#include "doc_ae_lexica.h"
#include "doc_html_lexica.h"
#include "doc_ae_syntax_analysis.h"
#include "doc_ae_synthesis.h"
#include "ide_console.h"
namespace ide::compiler
{
    using namespace doc;
    using std::filesystem::path;

    void print (entity e, array<str> & lines, int indent = 0, bool semicolon = true)
    {
        str line;

        for (auto && t : e.head) line += t.text + " "; line.truncate();

        if (e.info != "") line += " // " + e.info;

        semicolon &= e.body.empty() and not line.starts_with("#");

        if (line != "" and semicolon) line += ";";

        if (line != "") line = str(' ', indent) + line;
        
        if (line != "") lines += line;

        if (not e.body.empty())
        {
            bool inner_semicolon = e.kind != "enum" && e.kind != "declarator";
            bool outer_semicolon = e.kind == "enum" || e.kind == "declarator"
                || e.kind == "function" || e.kind == "class"
                || e.kind == "lambda"   || e.kind == "union";
            
            lines += str(' ', indent) + "{";

                for (auto && e : e.body) print (e, lines, indent + 4, inner_semicolon);

            lines += str(' ', indent) + "}"; if (outer_semicolon) lines.back() += ";";
        }

        line = "";

        for (auto && t : e.tail) line += t.text + " "; line.truncate();
        
        if (line != "") lines += line;
    }

    str load (std::filesystem::path path)
    {
        std::ifstream stream(path); str text = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};

        if (text.starts_with("\xEF" "\xBB" "\xBF"))
            text.upto(3).erase(); // UTF-8 BOM

        return text;
    }

    void add_std_library (path path)
    {
        ae::syntax::analysis::standard_library += path;
    }

    bool prepare (gui::text::console & console)
    {
        console << "Prepare library...";

        for (auto path : ae::syntax::analysis::standard_library)
            console << ae::syntax::analysis::proceed(path).log();

        console << "Done.";
        
        return true;
    }

    bool compile (path src, gui::text::console & console)
    {
        //console.clear();
        auto analysis = ae::syntax::analysis::proceed(src);
        if (analysis.log() != "") console << analysis.log();
        if (analysis.log.errors.size() > 0) return false;

        path cpp = src; cpp.replace_extension(".ae!.cpp");
        path obj = src; obj.replace_extension(".ae!.obj");
        path exe = src; exe.replace_extension(".ae!.exe");

        using namespace std::literals::chrono_literals;
        if (std::filesystem::exists(exe) and
            std::filesystem::last_write_time(exe) >
            analysis.compile_time)
            return true;

        console << "Synthesis...";
        array<entity> statements = ae::synthesis::proceed(analysis);

        array<str> lines;
        for (auto && s : statements)
            print(s, lines);

        {
            std::ofstream result(cpp);
            result << "\xEF" "\xBB" "\xBF"; // UTF-8 BOM
            for  (auto && line :lines) {
                result << line;
                result << "\n";
            }
        }

        path sdk_include = "c:\\Program Files (x86)"
            "\\Windows Kits\\10\\Include\\10.0.18362.0";

        path sdk_lib = "c:\\Program Files (x86)"
            "\\Windows Kits\\10\\Lib\\10.0.18362.0";

        path vc = "c:\\Program Files (x86)"
            "\\Microsoft Visual Studio\\2019\\Community\\VC"
            "\\Tools\\MSVC\\14.26.28801";

        path cl = vc / "bin\\Hostx64\\x64\\cl.exe";

        if (!std::filesystem::exists(cl)) {
            console << "<font color=#B00020>" "C++ compiler not found" "</font>";
            return false;
        }

        path cllog = "cl.log.txt";
        if (std::filesystem::exists(cllog)) 
            std::filesystem::remove(cllog);

        try
        {
            str flags = "/EHsc /Fo\"" + obj.string() + "\"";
            flags += " /I\"" + (vc/"include").string() + "\"";
            flags += " /I\"" + (sdk_include/"um").string() + "\"";
            flags += " /I\"" + (sdk_include/"ucrt").string() + "\"";
            str links = "/link /SUBSYSTEM:CONSOLE ";
            links += " /LIBPATH:\"" + (vc/"lib/x64").string() + "\"";
            links += " /LIBPATH:\"" + (sdk_lib/"um/x64").string() + "\"";
            links += " /LIBPATH:\"" + (sdk_lib/"ucrt/x64").string() + "\"";
            links += " /OUT:\"" + exe.string() + "\"";

            sys::process compile(cl, flags + " \"" + cpp.string() + "\" " + links,
            sys::process::options{.hidden = true, .out = cllog});
            if (!compile.wait(5*1000)) throw std::runtime_error(
                "Compilation takes too long.");
        }
        catch (const std::exception & e) {
            console << "<font color=#B00020>" + str(e.what()) + "</font>";
            return false;
        }

        // "fatal error LNK1104:"

        if (str sss = load(cllog);
            sss.find("error:") or sss.find("warning:") or
            sss.find("error ") or sss.find("warning "))
        {
            for (str s : sss.split())
                console << "<font color=#B00020>"
                + doc::html::lexica::encoded(s)
                + "</font>";
            return false;
        }

        return true;
    }

    void run (path src, gui::text::console & console)
    {
        try
        {
            console.clear();

            if (not compile(src, console)) return;

            path cpp = src; cpp.replace_extension(".ae!.cpp");
            path obj = src; obj.replace_extension(".ae!.obj");
            path exe = src; exe.replace_extension(".ae!.exe");

            console << "Run...";
            sys::process run(exe, "",
            sys::process::options{});
        }
        catch (const std::exception & e) {
            console << e.what();
            return;
        }
    }
}

