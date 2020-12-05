#pragma once
#include <string>
#include "data_ranges.h"
#include "data_collections.h"
#include "data_struct_array.h"
#include "data_unittest.h"
namespace data
{
    struct str
    {
        std::string data;

        using value_type = char;
        using iterator = contiguous_iterator<char>;
        using sentinel = contiguous_iterator<char>;
        using range_type = contiguous_collection_range<str>;
        auto range (iterator i, iterator j) { return range_type{*this, i, j}; }

        str () = default;
        str (const char    *  s) : data(s) {}
        str (const char8_t *  s) : data((char*)s) {}
        str (char c, int n =  1) { if (n > 0) data = std::string(n,c); }
        str (std::string_view s) { data = std::string(s); }

        str (const char * f, const char * l) { data = std::string(f, l); }

        explicit str (array<char> text) { data = std::string(
            text.data.data(), text.data.data() + text.size()); }

        explicit str (array<str> lines, str delimiter = "\n") {
            for (auto line : lines) { *this += line; *this += delimiter; }
            if (data.size() > 0) resize(data.size() - delimiter.data.size());
        }

        void resize  (int n) { data.resize (n); }
        void reserve (int n) { data.reserve(n); }

        auto begin () { return iterator{data.data()}; }
        auto end   () { return sentinel{begin() + (int)(data.size())}; }
        contiguous_range_impl(value_type);

        auto from (int n) { return from(begin()+n); }
        auto upto (int n) { return upto(begin()+n); }

        void operator += (char        c) { data += c; }
        void operator += (char const* s) { data += s; }
        void operator += (str  const& s) { data += s.data; }
        void operator += (str      && s) { data += std::move(s.data); }
        void operator += (input_range auto r) {
            for (const auto & e : r)
                (*this) += e;
        }

        void erase (iterator f, iterator l)
        {
            data.erase(
            data.begin() + (f - begin()),
            data.begin() + (l - begin()));
        }

        void insert (iterator i, range_type r)
        {
            data.insert(
            data.begin() + (i - begin()),
            r.begin().pointer,
            r.end().pointer);
        }

        #include "data_algo_random.h"
        #include "data_algo_resizing.h"
    };
}

#include "data_unittest.h"
namespace data::unittest
{
    void test_string ()
    {
        test("array.append");
        {
            array<int> a, b, c;
            oops( a += 1;            out(a) ) { "1" };
            oops( b += a;            out(b) ) { "1" };
            oops( b += b;            out(b) ) { "1, 1" };
            oops( c += std::move(b); out(c) ) { "1, 1" };
            oops( b += 2;            out(b) ) { "2" };
        }
        test("array.ranges");
        {
            array<int> a = {1, 2, 3};
            oops( out(a.from(0)) ) { "1, 2, 3" };
            oops( out(a.from(1)) ) { "2, 3" };
            oops( out(a.from(2)) ) { "3" };
            oops( out(a.from(3)) ) { "" };
            oops( out(a.from(0).upto(0)) ) { "" };
            oops( out(a.from(0).upto(1)) ) { "1" };
            oops( out(a.from(0).upto(2)) ) { "1, 2" };
            oops( out(a.from(0).upto(3)) ) { "1, 2, 3" };
            oops( out(a.from(1).upto(1)) ) { "" };
            oops( out(a.from(1).upto(2)) ) { "2" };
            oops( out(a.from(1).upto(3)) ) { "2, 3" };
            oops( out(a.from(2).upto(2)) ) { "" };
            oops( out(a.from(2).upto(3)) ) { "3" };
            oops( out(a.from(3).upto(3)) ) { "" };
            oops( out(a(0, 0)) ) { "" };
            oops( out(a(0, 1)) ) { "1" };
            oops( out(a(0, 2)) ) { "1, 2" };
            oops( out(a(0, 3)) ) { "1, 2, 3" };
            oops( out(a(1, 1)) ) { "" };
            oops( out(a(1, 2)) ) { "2" };
            oops( out(a(1, 3)) ) { "2, 3" };
            oops( out(a(2, 2)) ) { "" };
            oops( out(a(2, 3)) ) { "3" };
            oops( out(a(3, 3)) ) { "" };
        }
        test("array.replace");
        {
            array<int> a = {1, 2, 3};
            array<int> b = {4, 5, 6};
            oops( a.upto(0).replace_by(b.from(0)); out(a) ) { "1, 2, 3" };
            oops( a.upto(3).replace_by(b.from(3)); out(a) ) { "1, 2, 3" };
            oops( a.upto(1).replace_by(b.upto(1)); out(a) ) { "4, 2, 3" };
            oops( a.upto(1).replace_by(b.from(1)); out(a) ) { "5, 6, 2, 3" };
            oops( a.upto(2).replace_by(b.from(2)); out(a) ) { "6, 2, 3" };
            oops( a.upto(3).replace_by(b.upto(3)); out(a) ) { "4, 5, 6" };
        }
        test("");
    }
}
