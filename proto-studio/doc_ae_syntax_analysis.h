#pragma once
#include <chrono>
#include <filesystem>
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
#include "doc_ae_syntax_dependencies.h"
#include "doc_ae_syntax_typing.h"
namespace doc::ae::syntax::analysis
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    enum class state { ready, pass1, pass2, pass3, depend, resume };

    report events;

    struct data
    {
        path path;
        token name;
        statement module;
        dependencies dependencies;
        array<token>* tokens = nullptr;
        report log1; str ms1 = "0";
        report log2; str ms2 = "0";
        report log3; str ms3 = "0";
        sys::thread thread;
        state status;
        time time;

        void pass1 () // run after source changes
        {
            log1.clear();
            log2.clear();
            log3.clear();
            status = state::pass1;
            thread = [this](auto& cancel)
            {
                name.text = path.stem().string();
                module.names += &name;
                module.kind = "module";
                module.body =
                    schema(
                    parser(*tokens
                    , log1, cancel).output
                    , log1, cancel).output
                    ;
                scope (module, log1, cancel);

                dependencies.collect(module, cancel);
            };
        }

        void pass2 () // run after filesystem changes
        {
            log2.clear();
            log3.clear();
            status = state::pass2;
            thread = [this](auto& cancel)
            {
                dependencies.resolve(path, log2, cancel);
            };
        }

        void pass3 () // run when dependencies are ready
        {
            log3.clear();
            status = state::pass3;
            thread = [this](auto& cancel)
            {
                typing(log3).parse(module, cancel);
            };
        }

        void tick ()
        {
            try {
            switch(status){ default:
            break; case state::ready:
            break; case state::pass1:

                if (thread.done) {
                    thread.join();
                    thread.check();
                    ms1 = aux::ms(thread.duration);
                    if (not log1.errors.empty()) 
                    status = state::ready;
                    else pass2(); }

            break; case state::pass2:

                if (thread.done) {
                    thread.join();
                    thread.check();
                    ms2 = aux::ms(thread.duration);
                    if (not log2.errors.empty()) 
                    status = state::ready; else
                    status = state::depend; }

            break; case state::resume:

                pass3();

            break; case state::pass3:

                if (thread.done) {
                    thread.join();
                    thread.check();
                    ms3 = aux::ms(thread.duration);
                    status = state::ready;
                }
            }}
            catch (std::exception const& e) {
                events.error(e.what());
                status = state::ready;
            }
        }

        void abort ()
        {
            timing t0;
            status = state::ready;
            thread.stop = true;
            thread.join();
            thread.check();
            timing t1;
            if (t1-t0 > 25ms) events.debug(
            "syntax::analysis::data::abort "
            "took " + aux::ms(t1-t0) + " ms");
        }

        bool ready () { return status == state::ready; }

        void start () { pass1(); }

        void reanalyze ()
        {
            if (status != state::pass1)
            {
                abort();
                pass2();
            }
        }
    };

    std::unordered_map<path, data> repo;

    void tick (path path)
    {
        auto& data = repo[path];
        if (data.status != state::resume) {
            data.tick();
            return; }

        for (auto [dname, dpath]: data.dependencies.dependees)
        {
            auto& dependee = repo[dpath];
            if (not dependee.ready()) return;
            if (not dependee.log1.errors.empty()
            or  not dependee.log2.errors.empty()
            or  not dependee.log3.errors.empty()) {
            data.status = state::ready;
            return; }
        }

        for (auto [dname, dpath]: data.dependencies.dependees)
        {
            auto& dependee = repo[dpath];
            data.module.modules.emplace(
                dname, &dependee.module);
        }

        data.pass3();
    }
}

