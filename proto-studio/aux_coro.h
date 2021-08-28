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
#include <ranges>
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
    }
    catch(assertion_failed){}

    void test_coro2 () try
    {
        test("coro.task");
        {
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
