#pragma once
#include <algorithm>
#include "data.h"
namespace data::unittest
{
    using std::vector;
    using std::string;

    auto green (string s) { return "<font color=#008000>" + s + "</font>"; }
    auto gray  (string s) { return "<font color=#808080>" + s + "</font>"; }
    auto red   (string s) { return "<font color=#B00020>" + s + "</font>"; }

    string results, failure;
    vector<string> log;
    bool all_ok = true;

    using std::to_string;
    auto to_string (string s) { return s; }

    void out (string s) { log.push_back(s); }
    void out (input_range auto r, string delimiter = ", ")
    {
        string s;
        for (auto x : r) s += to_string(x) + delimiter;
        if (s != "") s.resize(s.size() - delimiter.size());
        out(s);
    }

    void commit () { for (auto s : log) results += s + "<br>"; log.clear(); }
    void print (string s) { out(s); commit(); }
    void test (string name)
    {
        static string summary;
        static string test_name;

        if (test_name != "")
        {
            summary += "<b>" + (failure == "" ?
            green(test_name) : red(test_name)) +
            "</b><br>" + failure;
        }

        all_ok &= failure == ""; failure = ""; log.clear();

        if (name == "")
        {
            test_name = "";
            results += "<br>";
            results += "<b>SUMMARY</b><br>";
            results += "<b>=======</b><br>";
            results += summary; summary = "";
            results += "<b>" + (all_ok ?
                green("ALL OK") :
                red("FAILED")) +
                "</b><br>";

            return;
        }
        if (test_name == "")
        {
            results = "";
            summary = "";
            all_ok = true;
        }
        print("");
        print("<b>" + name + "</b>");
        test_name = name;
    }

    bool check (string title, vector<string> true_log)
    {
        if (log.size() == 2 &&
            log[1] == "-----")
            log.resize(1);

        bool ok = log == true_log;
        
        string ss;
        if (log.size() > 1) ss += "<br>";
        for (auto s : log) ss += s + "<br>";
        log.clear();

        results += title + gray(" >>> ");
        results += ok ? green(ss) : red(ss);
        
        if (ok) return ok;
        failure += title + gray(" >>> ");
        failure += red(ss);
        return ok;
    }

    struct assertion_failed {};
    struct assertion {
        string title; bool throws;
        void operator == (vector<string> true_log) {
            if (! check(title, std::move(true_log))
                && throws) throw assertion_failed {};
        }
    };
    auto expect_(string title) { return assertion{title, false}; }
    auto assert_(string title) { return assertion{title, true }; }

#define oops(...) { __VA_ARGS__; out("-----"); }; expect_(#__VA_ARGS__) == vector<string>
#define ouch(...) { __VA_ARGS__; out("-----"); }; assert_(#__VA_ARGS__) == vector<string>

    struct element
    {
        string s;

       ~element () noexcept(false)         { out("dtor: " + s ); }
        element () noexcept(false) :s("_") { out("ctor: " + s ); }
        element (char           c) :s(1,c) { out("ctor: " + s ); }
        element (element const& e) :s(e.s) { out("copy: " + s ); }
        element (element     && e) :s(std::move(e.s))
                                           { out("move: " + s ); }
        void operator = (char           c) { out("assg: " + s + "=" + c   ); s = c;   }
        void operator = (element const& e) { out("copy: " + s + "=" + e.s ); s = e.s; }
        void operator = (element     && e) { out("move: " + s + "=" + e.s ); 
                                    s=std::move(e.s);}
    };
    struct copyable : element
    {
       ~ copyable             (                 ) = default;
         copyable             (                 ) = default;
         copyable             (element  const& e) : element(e) {}
         copyable             (element      && e) : element(std::move(e)) {}
         copyable             (copyable const& e) = default;
         copyable             (copyable     && e) = delete;
         copyable& operator = (copyable const& e) = default;
         copyable& operator = (copyable     && e) = delete;
    };
    struct moveable : element
    {
       ~ moveable             (                 ) = default;
         moveable             (                 ) = default;
         moveable             (element  const& e) : element (e) {}
         moveable             (element      && e) : element (std::move(e)) {}
         moveable             (moveable const& e) = delete;
         moveable             (moveable     && e) = default;
         moveable& operator = (moveable const& e) = delete;
         moveable& operator = (moveable     && e) = default;
    };
    struct makeable : element
    {
       ~ makeable             (                 ) = default;
         makeable             (                 ) = default;
         makeable             (element  const& e) : element (e) {}
         makeable             (element      && e) : element (std::move(e)) {}
         makeable             (makeable const& e) = delete;
         makeable             (makeable     && e) = delete;
         makeable& operator = (makeable const& e) = delete;
         makeable& operator = (makeable     && e) = delete;
    };
    using neither_copyable_nor_movable =  makeable;
    using movable =  moveable;
    using makable =  makeable;
}
