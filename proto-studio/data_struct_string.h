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
        using
        container = std::string;
        container data;

        using value_type = char;
        using iterator_= typename container::const_iterator;
        using iterator = typename container::iterator;
        using sentinel_= typename container::const_iterator;
        using sentinel = typename container::iterator;
        using range_type_= contiguous_collection_range_<str>;
        using range_type = contiguous_collection_range <str>;
        auto range (iterator_ i, iterator_ j) const { return range_type_{*this, i, j}; }
        auto range (iterator  i, iterator  j) /***/ { return range_type {*this, i, j}; }
        auto begin () /***/ { return data.begin(); }
        auto end   () /***/ { return data.end  (); }
        auto begin () const { return data.begin(); }
        auto end   () const { return data.end  (); }
        contiguous_range_impl(value_type);

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

        friend bool operator ==  (str const& l, str const& r) { return l.data == r.data; }
        friend auto operator <=> (str const& l, str const& r) {
            auto i = l.data.compare(r.data); return
                 i < 0 ? std::strong_ordering::less:
                 i > 0 ? std::strong_ordering::greater:
                         std::strong_ordering::equal;
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
            r.begin(),
            r.end());
        }
        void insert (iterator i, char c) { data.insert(i-begin(), 1, c); }
        void insert (int i, range_type r) { insert(begin()+i, r); }
        void insert (int i, char const* s) { data.insert(i, s); }
        void insert (int i, char c) { insert(begin()+i, c); }

        #include "data_algo_random.h"
        #include "data_algo_resizing.h"
    };
}

#include "data_unittest.h"
namespace data::unittest
{
    void test_string () try
    {
        test("string.substr");
        {
            ouch( out(str("").from(0).span(0)) ) { "" };
            ouch( out(str("").from(0).span(1)) ) { "" };
            ouch( out(str("").from(1).span(1)) ) { "" };

            oops( out(str("abc").from(1).span(0)) ) { "" };
            oops( out(str("abc").from(1).span(1)) ) { "b" };
            oops( out(str("abc").from(1).span(2)) ) { "bc" };
            oops( out(str("abc").from(1).span(3)) ) { "bc" };
            oops( out(str("abc").from(2)) ) { "c" };
            oops( out(str("abc").upto(2)) ) { "ab" };
        }
        test("string.compare");
        {
            oops( out(str("abc") == str("abc")) ) { "1" };
            oops( out(str("abc") ==     "abc" ) ) { "1" };
            oops( out(    "abc"  == str("abc")) ) { "1" };
            oops( out(str("abc") !=     "abz" ) ) { "1" };
            oops( out(    "abz"  != str("abc")) ) { "1" };
            oops( out(str("abc") <=     "abz" ) ) { "1" };
            oops( out(    "abz"  >= str("abc")) ) { "1" };

            oops( out(str("abc") != str("bcd").upto(3)) ) { "1" };
            oops( out(str("abc") <= str("bcd").upto(3)) ) { "1" };
            oops( out(str("abc").from(0) != str("bcd")) ) { "1" };
            oops( out(str("abc").from(0) <= str("bcd")) ) { "1" };
            oops( out(str("abc").from(0) != str("bcd").upto(3)) ) { "1" };
            oops( out(str("abc").from(0) <= str("bcd").upto(3)) ) { "1" };
            oops( out(str("abc").from(1) == str("bcd").upto(2)) ) { "1" };
        }
        test("string.insert");
        {
            oops( str s = "abc"; s.insert(0, "_"); out(s) ) { "_abc" };
            oops( str s = "abc"; s.insert(1, "_"); out(s) ) { "a_bc" };
            oops( str s = "abc"; s.insert(2, "_"); out(s) ) { "ab_c" };
            oops( str s = "abc"; s.insert(3, "_"); out(s) ) { "abc_" };
        }
        test("string.search");
        {
//            ASSERT_ANY_THROW(str("").find(""));
//            ASSERT_ANY_THROW(str("").find(str::one_of("")));
//            ASSERT_ANY_THROW(str("").find(str::one_not_of{ "" }));
//
//            ASSERT_ANY_THROW(str("abc").find(""));
//            ASSERT_ANY_THROW(str("abc").find(str::one_of("")));
//            ASSERT_ANY_THROW(str("abc").find(str::one_not_of{ "" }));
//
//            oops( out(str("").find("b").length, 0);
//            oops( out(str("").find(str::one_of("b")).length, 0);
//            oops( out(str("").find(str::one_not_of("b")).length, 0);
//
//            oops( out(str("b").find("b").length, 1);
//            oops( out(str("b").find(str::one_of{ "b" }).length, 1);
//            oops( out(str("b").find(str::one_not_of{ "b" }).length, 0);
//
///            oops( out(str("abccba").find(first("b")).offset()) ) { "1" };
//            oops( out(str("abccba").find("b").length, 1);
//            oops( out(str("abccba").find("d").length, 0);
//            oops( out(str("abccba").find("cc").offset, 2);
//            oops( out(str("abccba").find("cc").length, 2);
//
//            oops( out(str("abccba").find("b", str::start_from(1)).offset, 1);
//            oops( out(str("abccba").find("b", str::start_from(2)).offset, 4);
//            oops( out(str("abccba").find("b", str::start_from(5)).length, 0);
//            oops( out(str("abccba").find("b", str::start_from_end()).offset, 4);
//            oops( out(str("abccba").find("b", str::start_from_end(1)).offset, 4);
//            oops( out(str("abccba").find("b", str::start_from_end(2)).offset, 1);
//            oops( out(str("abccba").find("b", str::start_from_end(5)).length, 0);
        }
//        test("string.split");
//        {
//            ASSERT_ANY_THROW(str("").split_by(""));
//            ASSERT_ANY_THROW(str("|").split_by(""));
//            ASSERT_EQ(str("").split_by("|").size(), 1);
//            ASSERT_EQ(str("a").split_by("|").size(), 1);
//            ASSERT_EQ(str("|").split_by("|").size(), 2);
//            ASSERT_EQ(str("a|").split_by("|").size(), 2);
//            ASSERT_EQ(str("|b").split_by("|").size(), 2);
//            ASSERT_EQ(str("a|b").split_by("|").size(), 2);
//
//            oops( out(str("").split_by("|")[0], "");
//            oops( out(str("a").split_by("|")[0], "a");
//            oops( out(str("|").split_by("|")[0], "");
//            oops( out(str("|").split_by("|")[1], "");
//            oops( out(str("a|").split_by("|")[0], "a");
//            oops( out(str("a|").split_by("|")[1], "");
//            oops( out(str("|b").split_by("|")[0], "");
//            oops( out(str("|b").split_by("|")[1], "b");
//            oops( out(str("a|b").split_by("|")[0], "a");
//            oops( out(str("a|b").split_by("|")[1], "b");
//
//            str  a,b;
//            str("a|b").split_by("|", a, b, str::delimiter::to_the_left);  oops( out(a, "a|") << "split 1";
//            str("a|b").split_by("|", a, b, str::delimiter::to_the_left);  oops( out(b, "b" ) << "split 2";
//            str("a|b").split_by("|", a, b, str::delimiter::to_the_right); oops( out(a, "a" ) << "split 3";
//            str("a|b").split_by("|", a, b, str::delimiter::to_the_right); oops( out(b, "|b") << "split 4";
//        }
//        test("string.fragment");
//        {
//            EXPECT_TRUE(str("abc").starts_with(""));
//            EXPECT_TRUE(str("abc").starts_with("a"));
//            EXPECT_TRUE(str("abc").starts_with("ab"));
//            EXPECT_TRUE(str("abc").starts_with("abc"));
//            EXPECT_FALSE(str("abc").starts_with("abcd"));
//            EXPECT_FALSE(str("abc").starts_with("abd"));
//            EXPECT_FALSE(str("abc").starts_with("ad"));
//            EXPECT_FALSE(str("abc").starts_with("d"));
//
//            EXPECT_TRUE(str("abc").ends_with(""));
//            EXPECT_TRUE(str("abc").ends_with("c"));
//            EXPECT_TRUE(str("abc").ends_with("bc"));
//            EXPECT_TRUE(str("abc").ends_with("abc"));
//            EXPECT_FALSE(str("abc").ends_with("aabc"));
//            EXPECT_FALSE(str("abc").ends_with("aab"));
//            EXPECT_FALSE(str("abc").ends_with("aa"));
//            EXPECT_FALSE(str("abc").ends_with("a"));
//
//            EXPECT_TRUE(str("abccba").contains("cc"));
//            EXPECT_TRUE(str("abccba").contains(str::one_of("acc")));
//            EXPECT_TRUE(str("abccba").contains(str::one_not_of("acc")));
//
//            EXPECT_FALSE(str("abccba").contains_only("cc"));
//            EXPECT_FALSE(str("abccba").contains_only(str::one_of("acc")));
//            EXPECT_FALSE(str("abccba").contains_only(str::one_not_of("acc")));
//
//            EXPECT_TRUE(str("abccba").contains_only("abccba"));
//            EXPECT_TRUE(str("abccba").contains_only(str::one_of("abc")));
//            EXPECT_TRUE(str("abccba").contains_only(str::one_not_of("def")));
//
//            EXPECT_FALSE(str("").contains_only("abccba"));
//            EXPECT_TRUE(str("").contains_only(str::one_of("abc")));
//            EXPECT_TRUE(str("").contains_only(str::one_not_of("def")));
//        }
    }
    catch(assertion_failed){}
}
