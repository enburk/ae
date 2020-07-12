#pragma once
#include <fstream>
#include <filesystem>
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
namespace doc::ae::syntax::analysis
{
    struct data
    {
        report log;
        scope scope;
        array<statement> statements;
        std::filesystem::file_time_type time;
    };

    using path = std::filesystem::path;

    inline std::map<path, data> repo;

    inline array<path> standard_library;

    data proceed (path source, array<token> & tokens);

    const data & proceed (path source)
    {
        data & record = repo[source];
        auto & log = record.log;

        if (!std::filesystem::exists(source)) {
            record = data{};
            log.error(nullptr, source.string() + ": file doesn't exist");
            return record;
        }
        if (record.time > std::filesystem::last_write_time(source)) {
            return record;
        }

        record = data{};
        record.time = std::filesystem::last_write_time(source);

        std::ifstream stream(source); str s = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};

        if (s.starts_with("\xEF" "\xBB" "\xBF"))
            s.upto(3).erase(); // UTF-8 BOM

        array<token> tokens = lexica::parse(text{s});

        record = proceed(source, tokens);

        return record;
    };

    data proceed (path source, array<token> & tokens)
    {
        data record;
        auto & log = record.log;
        auto & statements = record.statements;

        statements = 
            statementor(log).proceed(
            parser(log).proceed(
            tokens));

        if (standard_library.find(source) ==
            standard_library.end())
            for (auto path : standard_library)
                record.scope.add(proceed(path).scope);

        for (auto & s : statements)
        {
            if (std::holds_alternative<pragma>(s.variant)) {
                auto & that = std::get<pragma>(s.variant);
                if (that.title->text == "using") {
                    that.title->kind = "keyword";
                }
            }
        }

        record.scope.named["global"].fill(statements, log);

        return record;
    }
}

