#pragma once
#include <fstream>
#include <filesystem>
#include "sys_aux.h"
#include "gui_widget_console.h"
#include "doc_ae_translator.h"
#include "app_ide_console.h"
namespace ide::compiler
{
    using std::filesystem::path;

    auto ext = [](path src, std::string ext){
        src.replace_extension(
        src.extension().string() + ext);
        return src;
    };
    auto cpp = [](path src){ return ext(src, ".cpp"); };
    auto obj = [](path src){ return ext(src, ".obj"); };
    auto exe = [](path src){ return ext(src, ".exe"); };

    str load (path path)
    {
        std::ifstream stream(path); str text = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};
        if (text.starts_with("\xEF" "\xBB" "\xBF"))
            text.upto(3).erase(); // UTF-8 BOM
        return text;
    }

    bool translate (path src, gui::console & console)
    {
        console.clear();
        auto & analysis =
        *doc::ae::syntax::analysis::datae[src];

        if (std::filesystem::exists(cpp(src)) and
            std::filesystem::last_write_time(cpp(src)) >
            analysis.time)
            return true;

        std::ofstream result(cpp(src));
        result << "\xEF" "\xBB" "\xBF"; // UTF-8 BOM
        for (auto && line : doc::ae::translator::proceed(analysis)) {
            result << line;
            result << "\n";
        }
        return true;
    }

    bool compile (path src, gui::console & console)
    {
        console << "Build...";

        path sdk_include = "c:\\Program Files (x86)"
            "\\Windows Kits\\10\\Include";//\\10.0.18362.0";
        for (std::filesystem::directory_iterator next(sdk_include),
            end; next != end; ++next)
            if (next->is_directory())
                sdk_include = *next;

        path sdk_lib = "c:\\Program Files (x86)"
            "\\Windows Kits\\10\\Lib";//\\10.0.18362.0";
        for (std::filesystem::directory_iterator next(sdk_lib),
            end; next != end; ++next)
            if (next->is_directory())
                sdk_lib = *next;

        path vc = "c:\\Program Files (x86)"
            "\\Microsoft Visual Studio\\2019\\Community\\VC"
            "\\Tools\\MSVC";//\\14.26.28801";
        for (std::filesystem::directory_iterator next(vc),
            end; next != end; ++next)
            if (next->is_directory())
                vc = *next;

        path cl = vc / "bin\\Hostx64\\x64\\cl.exe";

        if (!std::filesystem::exists(cl)) {
            console << "<font color=#B00020>"
                "C++ compiler not found"
                "</font>";
            return false;
        }

        std::filesystem::create_directories(".astudio");

        path cllog = ".astudio/cl.log.txt";
        if (std::filesystem::exists(cllog)) 
            std::filesystem::remove(cllog);

        try
        {
            str flags = "/std:c++latest"; flags +=
            src.extension() == ".ae!"  ? " /D \"SUBSYSTEM_CONSOLE\"" :
            src.extension() == ".ae!!" ? " /D \"SUBSYSTEM_WINDOWS\"" : "";
            flags += " /EHsc /Fo\".astudio/\"";
            flags += " /I\"" + (vc/"include").string() + "\"";
            flags += " /I\"" + (sdk_include/"um").string() + "\"";
            flags += " /I\"" + (sdk_include/"ucrt").string() + "\"";
            flags += " /I\"" + (sdk_include/"shared").string() + "\"";
            flags += " /TP"; // treat all sources as c++
            //flags += " /O2";
            str links = "/link"; links += 
            src.extension() == ".ae!"  ? " /SUBSYSTEM:CONSOLE" :
            src.extension() == ".ae!!" ? " /SUBSYSTEM:WINDOWS /entry:mainCRTStartup" : "";
            links += " /LIBPATH:\"" + (vc/"lib/x64").string() + "\"";
            links += " /LIBPATH:\"" + (sdk_lib/"um/x64").string() + "\"";
            links += " /LIBPATH:\"" + (sdk_lib/"ucrt/x64").string() + "\"";
            links += " /OUT:\"" + exe(src).string() + "\"";
            links += " /ManifestFile:\".astudio/app.exe.intermediate.manifest\"";
            links += " kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib";
            links += " shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib";// odbccp32.lib";

            str sources = "\"" + cpp(src).string() + "\"";
            sys::process compile(cl, flags + " " + sources + " " + links,
            sys::process::options{.out = cllog, .hidden = true});
            if (!compile.wait(5*1000)) throw std::runtime_error(
                "Compilation takes too long.");
        }
        catch (const std::exception & e) {
            console << "<font color=#B00020>"
            + str(e.what()) + "</font>";
            return false;
        }

        // "fatal error LNK1104:"

        if (str sss = load(cllog);
            sss.contains("error:") or sss.contains("warning:") or
            sss.contains("error ") or sss.contains("warning "))
        {
            for (str s : sss.split_by("\n")) {
                console << "<font color=#B00020>"
                + doc::html::encoded(s)
                + "</font>"; if (s != "")
                console << "<br>";
            }
            return false;
        }

        console << "Done.";

        return true;
    }

    void run (path src, gui::console & console)
    {
        if (not compile(src, console)) return;
        try
        {
            console << "Run...";
            sys::process run(exe(src), "",
            sys::process::options{});
            console << "Done.";
        }
        catch (const std::exception & e) {
            console << "<font color=#B00020>"
            + doc::html::encoded(e.what())
            + "</font>";
            return;
        }
    }
}

