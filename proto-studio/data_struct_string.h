#pragma once
#include <string>
#include "data_ranges.h"
#include "data_collections.h"
#include "data_struct_array.h"
#include "data_unittest.h"
namespace data
{
    struct str : std::string
    {
        using container = std::string;

        using value_type = char;
        using iterator_= typename container::const_iterator;
        using iterator = typename container::iterator;
        using sentinel_= typename container::const_iterator;
        using sentinel = typename container::iterator;
        using range_type_= contiguous_collection_range_<str>;
        using range_type = contiguous_collection_range <str>;
        auto range (iterator_ i, iterator_ j) const { return range_type_{*this, i, j}; }
        auto range (iterator  i, iterator  j) /***/ { return range_type {*this, i, j}; }
        contiguous_range_impl(value_type);

        str () = default;
        str (const char    *  s) : container(s) {}
        str (const char8_t *  s) : container((char*)s) {}
        str (char c, int n =  1) { if (n > 0) *this = std::string(n,c); }
        str (std::string_view s) : container(s) {}
        str (std::string      s) : container(std::move(s)) {}
        str (range_type_      r) : container(r.begin(), r.end()) {}
        str (range_type       r) : container(r.begin(), r.end()) {}

        str (const_iterator f, const_iterator l) { *this = std::string (f, l); }
        str (const char *   f, const char *   l) { *this = std::string (f, l); }

        explicit str (array<char> text) { *this = std::string(
            text.data.data(), text.data.data() + text.size()); }

        explicit str (array<str> lines, str delimiter = "\n") {
            for (auto line : lines) { *this += line; *this += delimiter; }
            if (size() > 0) resize(size() - delimiter.size());
        }

        auto from (int n) /***/ { return from(begin()+n); }
        auto upto (int n) /***/ { return upto(begin()+n); }
        auto from (int n) const { return from(begin()+n); }
        auto upto (int n) const { return upto(begin()+n); }

        void operator += (char        c) { container::operator+=(c); }
        void operator += (char const* s) { container::operator+=(s); }
        void operator += (str  const& s) { container::operator+=(s); }
        void operator += (str      && s) { container::operator+=(std::move(s)); }
        void operator += (input_range auto r) {
            for (const auto & e : r)
                (*this) += e;
        }

        void insert (iterator i, range_type r) {
            container::insert(i,
            r.begin(),
            r.end());
        }
        void insert (iterator i, char c) { container::insert(i-begin(), 1, c); }
        void insert (int i, range_type r) { insert(begin()+i, r); }
        void insert (int i, char const* s) { container::insert(i, s); }
        void insert (int i, char c) { insert(begin()+i, c); }

        #include "data_algo_random.h"
        #include "data_algo_resizing.h"

        enum class delimiter { exclude, to_the_left, to_the_right };


        bool split_by (str pattern, str& str1, str& str2, delimiter delimiter = delimiter::exclude) const
        {
            auto range = container::find (pattern); int n = range; int m = pattern.size();
            str1 = m > 0 ? str(upto (n     + (delimiter == delimiter::to_the_left  ? m : 0))) : *this;
            str2 = m > 0 ? str(from (n + m - (delimiter == delimiter::to_the_right ? m : 0))) : str{};
            return m > 0;
        }
        array<str> split_by (str pattern) const
        {
            array<str> result;
            if (size() == 0) return result;
            int start = 0; while (true) {
                auto range = container::find(pattern, start);
                bool found = range != std::string::npos;
                result += from(start).span(found ? range - start : size() - start);
                if (!found) range = size();
                start = range + pattern.size();
                if (start >= size()) { if (found) result += ""; break; }
            }
            return result;
        }
        bool contains (str pattern) const {
            return container::find(pattern) != std::string::npos;
        }
        int replace_all (str ffrom, str to) {
            int pos = 0, nn = 0;
            while (true) {
                auto range = container::find(ffrom, pos);
                if (range == std::string::npos) break;
                pos = range; from(range).span(ffrom.size()).replace_by(to);
                pos += to.size ();
                nn++;
            };
            return nn;
        }
        void truncate () { if (size() > 0) resize(size()-1); }
        void triml (const str & chars = " "){
            auto r = find_first_not_of(chars);
            bool found = r != std::string::npos;
            if (!found) clear(); else upto(r).erase();
        }
        void trimr (const str & chars = " "){
            auto r = find_last_not_of(chars);
            bool found = r != std::string::npos;
            if (!found) clear(); else from(r+1).erase();
        }
        void strip (const str & chars = " "){
             trimr(chars);
             triml(chars);
        }
        bool ascii_isalnum () const {
            for (char c : *this)
                if((c < '0') || ('9' < c &&
                    c < 'A') || ('Z' < c &&
                    c < 'a') || ('z' < c))
                    return false;
            return true;
        }

        static char ascii_tolower (char c) { return 'A' <= c && c <= 'Z' ? c - 'A' + 'a' : c; }
        static char ascii_toupper (char c) { return 'a' <= c && c <= 'z' ? c - 'a' + 'A' : c; }

        str ascii_lowercased () const { str s = *this; std::transform(s.begin(), s.end(), s.begin(), ascii_tolower); return s; }
        str ascii_uppercased () const { str s = *this; std::transform(s.begin(), s.end(), s.begin(), ascii_toupper); return s; }
    };

    namespace unicode
    {
        array<str> glyphs (str s)
        {
            array<str> glyphs;
            glyphs.reserve(s.size());

            auto check = [&s](auto i){ if (i == s.end())
            throw std::runtime_error("unicode: broken UTF-8"); };

            for (auto i = s.begin(); i != s.end(); )
            {
                char c = *i++; str g = c;
                uint8_t u = static_cast<uint8_t>(c);
                if ((u & 0b11000000) == 0b11000000) { check(i); g += *i++; // 110xxxxx 10xxxxxx
                if ((u & 0b11100000) == 0b11100000) { check(i); g += *i++; // 1110xxxx 10xxxxxx 10xxxxxx
                if ((u & 0b11110000) == 0b11110000) { check(i); g += *i++; // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                }}}
                glyphs += g;
            }
            return glyphs;
        }

        int length (str s)
        {
            int n = 0;
            for (auto i = s.begin(); i != s.end(); )
            {
                char c = *i++; n++;
                uint8_t u = static_cast<uint8_t>(c);
                if ((u & 0b11000000) == 0b11000000) { if (i == s.end()) break; n++; i++;
                if ((u & 0b11100000) == 0b11100000) { if (i == s.end()) break; n++; i++;
                if ((u & 0b11110000) == 0b11110000) { if (i == s.end()) break; n++; i++;
                }}}
            }
            return n;
        }
    }
}
namespace std
{
    template <> struct hash<data::str>
    {
        std::size_t operator()(const data::str & s) const
        {
            using std::size_t;
            using std::hash;
            using std::string;
            return hash<string>()(s);
        }
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
