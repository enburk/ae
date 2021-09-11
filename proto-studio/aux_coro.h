#pragma once
#ifdef __clang__
#include "aux_unittest.h"
namespace aux::unittest
{
	void test_coro1 () {}
	void test_coro2 () {}
	void test_coro3 () {}
}
#else
#include <coroutine>
#include <experimental/generator>

namespace aux
{
    using std::ranges::input_range;
    using std::ranges::random_access_range;
    using std::experimental::generator;

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

    template<class T = void>
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

        T operator()() {
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
    task<int> sum (task<int>&& a, task<int>&& b) {
        co_return (co_await a + co_await b); }

    void test_coro2 () try
    {
        test("coro.task");
        {
            int a = sum(four(), four())();
            int b = sum(four(), five())();
            oops(out(a)) { "8" };
            oops(out(b)) { "9" };
//            oops(out(co_await sum{four{}, four{}})) { "8" };
//            oops(out(co_await sum{four{}, five{}})) { "9" };
        }
    }
    catch(assertion_failed){}

    void test_coro3 () try
    {
        test("coro.task");
        {
        }
    }
    catch(assertion_failed){}
}

#endif
