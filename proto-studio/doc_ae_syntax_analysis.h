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

    report events;

    enum class state { ready, running, depend };

    auto now () { return std::chrono::high_resolution_clock::now(); }

    auto ms (auto t) { return std::to_string(gui::time(t).ms); }

    struct data
    {
        scope scope;
        scopes scopes;
        array<token> * tokens;
        array<statement> statements;
        array<std::pair<str, path>> dependees;
        array<token*> dependencies;
        report log1, log2, log3;
        bool passed2 = false;
        bool passed3 = false;
        std::atomic<int> running = 0;
        std::atomic<bool> done = false;
        std::atomic<bool> stop = false;
        std::thread thread;
        time time;
        path path;

        void pass1 ()
        {
            // auto t0 = now();

            log1.clear();
            statements =
                schema(
                parser(*tokens
                , log1, stop).output
                , log1, stop).output
                ;
            scope = decltype(scope)(
                statements,
                log1);
            dependencies =
            dependencies::collect(statements);
            done = true;

            // auto t1 = now(); log1.debug("pass1 " + ms(t1-t0) + " ms");
        }

        void pass2 ()
        {
            // auto t0 = now();

            log2.clear();
            dependees =
            dependencies::resolve(
                path.parent_path(),
                dependencies, log2);
            done = true;

            // auto t1 = now(); log2.debug("pass2 " + ms(t1-t0) + " ms");
        }

        void pass3 ()
        {
            // auto t0 = now();

            log3.clear();
            typing::parse(statements, scopes, log3);
            done = true;

            // auto t1 = now(); log3.debug("pass3 " + ms(t1-t0) + " ms");
        }

        void run1 ()
        {
            //events.debug("pass1 " + path.string());
            stop = false; done = false; running = 1;
            thread = std::thread([this]{ pass1(); });
        }
        void run2 ()
        {
            if (passed2) return; passed2 = true;
            //events.debug("pass2 " + path.string());
            stop = false; done = false; running = 2;
            thread = std::thread([this]{ pass2(); });
        }
        void run3 ()
        {
            if (passed3) return; passed3 = true;
            //events.debug("pass3 " + path.string());
            stop = false; done = false; running = 3;
            thread = std::thread([this]{ pass3(); });
        }

        state tick ()
        {
            if (running == 0) return state::ready;
            if (not done) return state::running;
            else thread.join();

            if (running == 1) {
                running =  0;
                passed2 = false;
                passed3 = false; run2();
                return state::running;
            }
            if (running == 2) {
                running =  0;
                return state::depend;
            }
            if (running == 3) { 
                running =  0;
                time = time::clock::now();
                return state::ready;
            }

            throw std::logic_error
            ("nonsense");
        }
    };

    std::map<path, data*> datae;

    void start (path path, data & data, array<token> & tokens)
    {
        datae[path] = &data;
        data.tokens = &tokens;
        data.path = path;
        data.run1();
    }

    void abort (path path)
    {
        auto data = datae[path];
        if (not data) return;
        data->stop = true;
        data->running = 0;
        auto t0 = now();
        if (data->thread.joinable())
            data->thread.join();
        auto t1 = now(); if (gui::time(t1-t0).ms > 25)
        events.debug("syntax::analysis::abort took "
        + ms(t1-t0) + " ms");
    }

    void prereanalize (path path)
    {
        auto data = datae[path];
        if (not data) return;
        data->passed2 = false;
        data->passed3 = false;
        if (data->running == 2 or
            data->running == 3)
            abort(path);
    }

    void reanalize (path path)
    {
        auto data = datae[path];
        if (not data) return;
        if (data->running != 0) return;
        data->run2();
    }

    auto tick (path path)
    {
        auto data = datae[path];
        if (not data) return state::ready;
        return data->tick();
    }

    void resume (path path)
    {
        auto data = datae[path];
        auto core = datae[std::filesystem::
            current_path() / "library" / "core.ae"];

        static scope coreless;
        auto & scopes = data->scopes;
        scopes.core = core ? &core->scope : &coreless;
        scopes.scope = &data->scope;

        for (auto [name, path] : data->dependees)
            scopes.outscope.emplace(std::pair{
                name, &datae[path]->scope});

        data->run3();
    }

    void remove (path path)
    {
        datae.erase(path);
    }
}

