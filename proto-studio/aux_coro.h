#pragma once
#include <coroutine>
#include <experimental/generator>
#include <future>

using std::ranges::input_range;
using std::ranges::random_access_range;
using std::experimental::generator;

namespace aux
{
     template
    <typename X>
    generator<X> operator_plus (generator<X>&& g, X ending)
    {
        for (auto x: g) co_yield x; co_yield ending;
    }

    template<input_range R>
    auto enumerate (R& r) -> generator<
    std::pair<int, typename R::value_type>>
    {
        int n = 0;
        for (auto& x : r)
            co_yield {n++, x};
    }

    template<input_range R>
    auto enumerate (R& r, typename R::value_type end) -> generator<
    std::pair<int, typename R::value_type>>
    {
        int n = 0;
        for (auto& x : r)
            co_yield {n++, x};
        co_yield {n, end};
    }

    template<class T=nothing>
    struct [[nodiscard]] task
    {
        struct promise_type {
            using handle_type = std::coroutine_handle<promise_type>;
            std::variant<std::monostate, T, std::exception_ptr> result;
            std::coroutine_handle<> previous; // who waits on this coroutine
            auto get_return_object  () { return task{handle_type::from_promise(*this)}; }
            void return_value(T value) { result.template emplace<1>(std::move(value)); }
            void unhandled_exception() { result.template emplace<2>(std::current_exception()); }
            auto initial_suspend() { return std::suspend_always{}; }
            auto final_suspend() noexcept {
                struct final_awaiter {
                    void await_resume() noexcept {}
                    bool await_ready () noexcept { return false; }
                    std::coroutine_handle<> await_suspend(handle_type me) noexcept {
                    // final_awaiter::await_suspend is called when the execution of the
                    // current coroutine (referred to by 'me') is about to finish.
                    // If the current coroutine was resumed by another coroutine via
                    // co_await get_task(), a handle to that coroutine has been stored
                    // as h.promise().previous. In that case, return the handle to resume
                    // the previous coroutine. Otherwise, return noop_coroutine(),
                    // whose resumption does nothing.
                        auto previous = me.promise().previous;
                        if (previous) return previous;
                        return std::noop_coroutine();
                    }
                };
                return final_awaiter{};
            }
        };

        using handle_type = promise_type::handle_type;
        explicit task(handle_type h) : handle(h) {}
        task(task&& rhs) : handle(rhs.handle) { rhs.handle = nullptr; }
       ~task() { if (handle) handle.destroy(); }
        handle_type handle;

        auto operator co_await() {
            struct awaiter {
                handle_type handle;
                bool await_ready() { return false; }
                auto await_resume() -> T {  
                    auto& r = handle.promise().result;
                    if (r.index() == 1) return std::get<1>(r);
                    std::rethrow_exception(std::get<2>(r));
                }
                auto await_suspend(std::coroutine_handle<> h) {
                    handle.promise().previous = h;
                    return handle;
                }
            };
            return awaiter{handle};
        }

        T operator()() const {
            handle.resume();
            auto& r = handle.promise().result;
            if (r.index() == 1) return std::get<1>(r);
            std::rethrow_exception(std::get<2>(r));
        }
    };
}

#include "aux_unittest.h"
namespace aux::unittest
{
    void test_coro1 () try
    {
        test("coro.generator");
        {
			auto fibs = []() -> generator<int> {
				int a = 0, b = 1; while (true) {
					co_yield a; b += std::
						exchange(a, b); } } ();

            auto i = fibs.begin();
            oops(out(*++i)) { "1" };
            oops(out(*++i)) { "1" };
            oops(out(*++i)) { "2" };
            oops(out(*++i)) { "3" };
            oops(out(*++i)) { "5" };
            oops(out(*++i)) { "8" };

			auto nats = []() -> generator<int> {
				int n = 0; while (true)
					co_yield n++; } ();

            int N = 100;
            int sum = 0;
            for (int n : nats) {
                if (n > N) break;
                sum += n; }
			
            oops(out(sum)) { std::to_string(N*(N+1)/2) };
        }

        test("coro.generator.plus");
        {
            auto abc = []() -> generator<char> {
                co_yield 'a';
                co_yield 'b';
                co_yield 'c';
            };

            string s1; for (char c: abc()) s1 += c;
            string s2; for (auto z = abc(); char c: operator_plus(std::move(abc()), ' ')) s2 += c;
            oops(out(s1)) { "abc"  };
            oops(out(s2)) { "abc " };
        }

        test("coro.enumerate.1");
        {
            string s;
            string cc = "abc";
            for (auto [i, c] : enumerate(cc))
            s += std::to_string(i) + c + ' ';
            oops(out(s)) { "0a 1b 2c " };
        }

        test("coro.enumerate.2");
        {
            string s;
            vector<int> nn = {0, 1, 2};
            for (auto [i, n] : enumerate(nn))
            s += std::to_string(i) + std::to_string(n) + ' ';
            oops(out(s)) { "00 11 22 " };
        }

        test("coro.enumerate.3");
        {
            string s;
            vector<string> lines = {"", "a", "bc"};
            for (auto [i, line] : enumerate(lines))
                for (auto [j, c] : enumerate(line))
                    s += std::to_string(i) + ":" +
                         std::to_string(j) + " " +
                         c + " ";
            oops(out(s)) { "1:0 a 2:0 b 2:1 c " };
        }

        test("coro.enumerate.4");
        {
            string s;
            vector<string> lines = {"", "a", "bc"};
            for (auto [i, line] : enumerate(lines, "d"))
                for (auto [j, c] : enumerate(line, '-'))
                    s += std::to_string(i) + ":" +
                         std::to_string(j) + " " +
                         c + " ";
            oops(out(s)) { "0:0 - 1:0 a 1:1 - 2:0 b 2:1 c 2:2 - 3:0 d 3:1 - " };
        }
    }
    catch(assertion_failed){}

    task<int> four () { co_return 4; }
    task<int> five () { co_return 5; }
    task<int> boom () { throw std::runtime_error("boom"); co_return 0; }
    task<int> sum (task<int>&& a, task<int>&& b) {
        co_return (co_await a + co_await b); }

    string safe (auto&& task)
    try { return std::to_string(task()); }
    catch(std::exception const& e) { return e.what(); }

    void test_coro2 () try
    {
        test("coro.task");
        {
            oops(out(four()())) { "4" };
            oops(out(five()())) { "5" };
            oops(out(sum(four(), four())())) { "8" };
            oops(out(sum(four(), five())())) { "9" };

            oops(out(safe(four()))) { "4" };
            oops(out(safe(boom()))) { "boom" };
            oops(out(safe(sum(boom(), four())))) { "boom" };
            oops(out(safe(sum(four(), boom())))) { "boom" };
        }
    }
    catch(assertion_failed){}

    std::mutex mutex;

    task<> sleep (string s, int ms) {
        std::this_thread::sleep_for(
        std::chrono::milliseconds(ms));
        std::lock_guard lock{mutex};
        out(s + " awaken after " +
        std::to_string(ms) + " ms");
        co_return nothing{}; }

    auto launch (task<>&& t) {
        return std::async(
        std::launch::async,
        [t=std::move(t)]()
        { t(); }); }

    task<> await (task<>&& a, task<>&& b) {
        auto future1 = launch(std::move(a));
        auto future2 = launch(std::move(b));
        auto t0 = std::chrono::high_resolution_clock::now();
        future1.get();
        future2.get();
        auto t1 = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<
                  std::chrono::milliseconds>(t1-t0 +
                  std::chrono::microseconds(500));
        std::lock_guard lock{mutex};
        out("waited for " +
        std::to_string(ms.count()) + " ms");
        co_return nothing{}; }

    void test_coro3 () try
    {
        test("coro.async");
        {
            oops(sleep("0", 0)()) { "0 awaken after 0 ms" };
            oops(sleep("1", 1)()) { "1 awaken after 1 ms" };

            oops(await(sleep("A", 100), sleep("B", 200))()) {
                "A awaken after 100 ms",
                "B awaken after 200 ms",
                "waited for 200 ms" };


        }
    }
    catch(assertion_failed){}
}
