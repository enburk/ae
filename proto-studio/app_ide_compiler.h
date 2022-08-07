#pragma once
#include <fstream>
#include <filesystem>
#include "abc.h"
#include "doc_ae_translator.h"
#include "app_ide_console.h"
namespace ide::compiler
{
    using std::filesystem::path;

    auto ext (path src, std::string ext){
        src.replace_extension(
        src.extension().string() + ext);
        return src; }

    auto hxx (path src){ return ext(src, ".h++"); }
    auto hpp (path src){ return ext(src, ".hpp"); }
    auto cxx (path src){ return ext(src, ".c++"); }
    auto cpp (path src){ return ext(src, ".cpp"); }
    auto obj (path src){ return ext(src, ".obj"); }
    auto exe (path src){ return ext(src, ".exe"); }

    str load (path path)
    {
        std::ifstream stream(path); str text = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};
        if (text.starts_with("\xEF" "\xBB" "\xBF"))
            text.upto(3).erase(); // UTF-8 BOM
        return text;
    }

    namespace analysis = doc::ae::syntax::analysis;

    array<path> translate (analysis::data& data, gui::console& console, bool main) try
    {
        array<path> inners;
        array<path> outers;
        array<path> cxxs;

        using namespace std::filesystem;

        if (exists(cxx(data.path)))
           cxxs += cxx(data.path);

        for (auto [name, path]: data.dependencies.dependees)
        {
            cxxs += translate(analysis::repo[path], console, false);

            path = hpp(path);

            if (path.parent_path() ==
            data.path.parent_path() /
            data.name.text.c_str())
            inners += path; else
            outers += path;
        }

        auto path = main ? 
            cpp(data.path):
            hpp(data.path);

        // if (not exists(path)
        // or last_write_time(path) < data.time)
        {
            console << data.path.string();

            std::ofstream result(path);
            result << "\xEF" "\xBB" "\xBF"; // UTF-8 BOM
            result << "#pragma once\n";
            result << "#include <span>\n";
            result << "#include <cstdint>\n";

            for (auto p: outers)
            result << "#include \"" +
            p.string() + "\"\n";

            result << "namespace ae\n";
            result << "{\n";

            for (auto p: inners)
            result << "#include \"" +
            p.string() + "\"\n";

            for (auto&& line:
            doc::ae::translator::proceed(data, main)) {
                result << "    ";
                result << line;
                result << "\n";
            }
            result << "}\n";

            if (exists(hxx(data.path)))
            result << "#include \"" +
            hxx(data.path).string() +
            "\"\n";

            if (main)
            for (auto p: cxxs)
            result << "#include \"" +
            p.string() + "\"\n";
        }
        return cxxs;
    }
    catch(...)
    {
        auto path = main ? 
            cpp(data.path):
            hpp(data.path);
        if (exists(path))
            remove(path);
        throw;
    }

    bool translate (path src, gui::console& console) try
    {
        console.clear();
        translate(analysis::repo[src], console, true);
        return true;
    }
    catch(std::exception const& e) {
        console << red(doc::html::
        encoded(e.what()));
        return false; }

    bool compile (path src, gui::console& console, auto& cancel)
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

        path vc = "c:\\Program Files"
            "\\Microsoft Visual Studio\\2022\\Community\\VC"
            "\\Tools\\MSVC";//\\14.26.28801";
        for (std::filesystem::directory_iterator next(vc),
            end; next != end; ++next)
            if (next->is_directory())
                vc = *next;

        path cl = vc / "bin\\Hostx64\\x64\\cl.exe";

        if (not std::filesystem::exists(cl)) {
            console << red("C++ compiler not found");
            return false; }

        std::filesystem::create_directories(".astudio");

        path cllog = ".astudio/cl.log.txt";
        if (std::filesystem::exists(cllog)) 
            std::filesystem::remove(cllog);

        try
        {
            str ext = src.extension().string();

            str flags = "/std:c++latest"; flags +=
            ext == ".ae!"  ? " /D \"SUBSYSTEM_CONSOLE\"" :
            ext == ".ae!!" ? " /D \"SUBSYSTEM_WINDOWS\"" : "";
            flags += " /EHsc /Fo\".astudio/\"";
            flags += " /I\"" + (vc/"include").string() + "\"";
            flags += " /I\"" + (sdk_include/"um").string() + "\"";
            flags += " /I\"" + (sdk_include/"ucrt").string() + "\"";
            flags += " /I\"" + (sdk_include/"shared").string() + "\"";
            flags += " /TP"; // treat all sources as c++
            //flags += " /O2";

            str links = "/link"; links += 
            ext == ".ae!"  ? " /SUBSYSTEM:CONSOLE" :
            ext == ".ae!!" ? " /SUBSYSTEM:WINDOWS /entry:mainCRTStartup" : "";
            links += " /LIBPATH:\"" + (vc/"lib/x64").string() + "\"";
            links += " /LIBPATH:\"" + (sdk_lib/"um/x64").string() + "\"";
            links += " /LIBPATH:\"" + (sdk_lib/"ucrt/x64").string() + "\"";
            links += " /OUT:\"" + exe(src).string() + "\"";
            links += " /ManifestFile:\".astudio/app.exe.intermediate.manifest\"";
            links += " kernel32.lib user32.lib gdi32.lib winspool.lib";
            links += " comdlg32.lib advapi32.lib shell32.lib";
            links += " ole32.lib oleaut32.lib uuid.lib";
            links += " odbc32.lib";// odbccp32.lib";

            str sources = "\"" + cpp(src).string() + "\"";
            sys::process compile(cl, flags + " " + sources + " " + links,
            sys::process::options{.out = cllog, .hidden = true});
            if (!compile.wait(5*1000)) throw std::runtime_error(
                "Compilation takes too long.");
        }
        catch (const std::exception & e) {
            console << red(str(e.what()));
            return false; }

        // "fatal error LNK1104:"

        if (str sss = load(cllog);
            sss.contains("error:") or sss.contains("warning:") or
            sss.contains("error ") or sss.contains("warning "))
        {
            for (str s: sss.split_by("\n"))
            console << red(doc::html::encoded(s));
            return false;
        }

        console << "Done.";

        return true;
    }

    void run (path src, gui::console& console, auto& cancel)
    {
        try
        {
            if (not compile(src,
                console, cancel))
                return;

            console << "Run...";
            sys::process run(exe(src), "",
            sys::process::options{});
            console << "Done.";
        }
        catch (std::exception const& e) {
            console << red(doc::html::
                encoded(e.what())); }
    }
}

