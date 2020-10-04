#pragma once
#include "doc_text_repo.h"
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scheme.h"
#include "doc_ae_syntax_analysis.h"
namespace doc::ae::syntax::analysis
{
    void pass1 (data & record, path source)
    {
        time edit_time;
        text_model model;
        time now = time::clock::now();
        doc::repo::edittime(source, edit_time).value();

        if (record.compile_time >= edit_time)
        {
            bool up_to_date = true;

            for (auto path : record.dependencies)
            {
                doc::repo::edittime(path, edit_time).value();
                if (sources[path].compile_time < edit_time) {
                    up_to_date = false;
                    break;
                }
            }

            if (up_to_date) return;// false;
        }

        doc::repo::load(source, model).value();

        record = data{};
        record.source = source;
        record.compile_time = now;
        record.log.info(nullptr, source.string());
        record.tokens = lexica::parse(model);

        if (source.string().ends_with(".c++")) {
            report log = cpp::syntax::analysis::proceed(source, record.tokens).log;
            record.log.messages += log.messages;
            record.log.errors += log.errors;
            record.cpp = true;
            return;// true;
        }

        pass1 (record, source, record.tokens);
        return;// true;
    };

    void pass1 (data & record, path source, array<token> & tokens)
    {
        auto & log = record.log;

        record.statements = statementor(parser(tokens
            , log).output
            , log).output
            ;

        if (not log.errors.empty()) return;

        if (not standard_library.contains(source))
        {
            for (auto path : standard_library)
            {
                record.dependencies.try_emplace(path);
                auto & data = sources[path]; pass1(data, path);
                for (auto p : data.dependencies) if (p != source)
                    record.dependencies.try_emplace(p);
            }
        }

        for (auto & s : record.statements)
        {
            if (std::holds_alternative<pragma>(s.variant)) {
                auto & that = std::get<pragma>(s.variant);
                if (that.title->text == "using")
                {
                    std::string fn;
                    if (aux::got<named_pack>(that.arg.variant)) { auto units =
                        std::get<named_pack>(that.arg.variant).units;
                        if (units.size() != 1) continue;
                        if (units[0].arguments.size() != 0) continue;
                        fn = units[0].identifier->text;
                    }
                    else continue;
                    if (not fn.ends_with(".c++")) fn += ".ae";
                    auto path = source.parent_path() / fn;
                    if (record.dependencies.contains(path)) continue;

                    record.dependencies.try_emplace(path);
                    auto & data = sources[path]; pass1(data, path);
                    for (auto p : data.dependencies) if (p != source)
                        record.dependencies.try_emplace(p);
                }
            }
        }

        record.scope.fill(record.statements);
    }
}

