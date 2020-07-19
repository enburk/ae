#pragma once
#include <fstream>
#include <filesystem>
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
namespace doc::ae::syntax::analysis
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    struct data
    {
        report log;
        scope scope;
        array<token> tokens;
        array<statement> statements;
        array<path> dependencies;
        time time;
    };

    inline std::map<path, data> repo;

    inline array<path> standard_library;

    void pass1 (data & record, path source, array<token> &);
    void pass1 (data & record, path source)
    {
        auto & log = record.log;

        if (!std::filesystem::exists(source)) { record = data{};
            log.error(nullptr, source.string() + ": file doesn't exist");
            return;
        }
        // prevent circular dependencies recursion
        if (record.time == time{} // treat as "now"
        or  record.time > std::filesystem::last_write_time(source)) {
            return;
        }

        record = data{};
        record.time = std::filesystem::last_write_time(source);

        std::ifstream stream(source); str s = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};

        if (s.starts_with("\xEF" "\xBB" "\xBF"))
            s.upto(3).erase(); // UTF-8 BOM

        record.tokens = lexica::parse(text{s});

        pass1 (record, source, record.tokens);
    };

    void pass1 (data & record, path source, array<token> & tokens)
    {
        auto & log = record.log;

        record.statements = 
            statementor(log).proceed(
            parser(log).proceed(
            tokens));

        if (not log.messages.empty()) return;

        if (not standard_library.contains(source))
        {
            for (auto path : standard_library)
            {
                record.dependencies += path;
                auto data = repo[path]; pass1(data, path);
                if (not data.log.messages.empty()) {
                    log.error(nullptr, path.string());
                    log.messages += data.log.messages;
                    return;
                }
            }
        }

        for (auto & s : record.statements)
        {
            if (std::holds_alternative<pragma>(s.variant)) {
                auto & that = std::get<pragma>(s.variant);
                if (that.title->text == "using")
                {
                    std::string s = that.param->text;
                    s.pop_back(); s.erase(0,1); s += ".ae";
                    auto path = source.parent_path() / s;
                    if (record.dependencies.contains(path)) continue;
                    record.dependencies += path;
                    auto data = repo[path]; pass1(data, path);
                    if (not data.log.messages.empty()) {
                        log.error(that.param, "using " + that.param->text);
                        log.messages += data.log.messages;
                        return;
                    }
                }
            }
        }

        record.scope.named["global"].fill(record.statements, log);
    }

    void pass2 (data & record)
    {
        auto & log = record.log;

        if (not log.messages.empty()) return;

        //for (auto path : record.dependencies)
        //    record.scope.add(repo[path].scope);
    }

    // run/compilation button
    const data & proceed (path source)
    {
        data & record = repo[source];
        pass1 (record, source);
        pass2 (record);
        return record;
    }

    // live editor analysis
    const data & proceed (path source, array<token> & tokens)
    {
        data & record = repo[source]; record.time = time(); // treat as "now"
        pass1 (record, source, tokens);
        pass2 (record);
        return record;
    }
}

