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
        scope scope;
        scopes scopes;
        array<token>* tokens;
        array<statement> statements;
        dependencies dependencies;
        report log1, log2, log3;
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
                statements =
                    schema(
                    parser(*tokens
                    , log1, cancel).output
                    , log1, cancel).output
                    ;
                scope = decltype(scope)(log1, statements);

                dependencies.collect(statements);
            };
        }

        void pass2 () // run after filesystem changes
        {
            log2.clear();
            log3.clear();
            status = state::pass2;
            thread = [this](auto& cancel)
            {
                dependencies.resolve(path, log2);
            };
        }

        void pass3 () // run when dependencies scopes are ready
        {
            log3.clear();
            status = state::pass3;
            thread = [this](auto& cancel)
            {
                typing::parse(statements, scopes, log3);
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
                    if (not log1.errors.empty()) 
                    status = state::ready;
                    else pass2(); }

            break; case state::pass2:

                if (thread.done) {
                    thread.join();
                    thread.check();
                    if (not log2.errors.empty()) 
                    status = state::ready; else
                    status = state::depend; }

            break; case state::resume:

                pass3();

            break; case state::pass3:

                if (thread.done) {
                    thread.join();
                    thread.check();
                    status = state::ready;
                }
            }}
            catch (std::exception const& e) {
            events.error(e.what()); }
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

        void preanalyze () {}

        void analyze ()
        {
            if (status == state::pass1) return;
            if (status == state::pass2
            or  status == state::pass3) {
                thread.join();
                thread.check(); }
            pass2();
        }
    };

    std::unordered_map<path, data> repo;

            //events.debug("pass1 " + path.string());
            // auto t1 = now(); log3.debug("pass3 " + ms(t1-t0) + " ms");

}

