#pragma once
#include <string> 
#include <variant> 
#include <functional>
#include "aux_array.h"

struct str : public std::string
{
    using base = std::string;

    str (               ) : base ( ) {}
    str (const base & s ) : base (s) {}
    str (      base &&s ) : base (std::move(s)) {}
    str (const char * s ) : base (s) {}
    str (char c, int n=1) { if (n > 0) *this = base (n,c); }
    str (const str &  s ) = default;
    str (      str && s ) = default;
    str (const_iterator f, const_iterator l) : base (f, l) {}

    explicit str (array<str> lines) {
        for (auto line : lines) {
            *this += line;
            *this += '\n'; }
        if (*this != "") pop_back();
    }

    ////////////////////////////////////////////////////////////////////////////

    int size () const { return (int) base::size (); }

    str & operator  = (const str &  s) { base::operator  = (s); return *this; }
    str & operator  = (      str && s) { base::operator  = (std::move(s)); return *this; }
    str & operator += (const str &  s) { base::operator += (s); return *this; }
    str & operator += (      str && s) { base::operator += (std::move(s)); return *this; }

    static const int max = std::numeric_limits<int>::max ();

    struct range { int pos, size; bool empty () const noexcept { return size <= 0; } };

    struct start { int offset; bool from_end; };
    static start start_from     (int offset = 0){ return start {offset, false}; }
    static start start_from_end (int offset = 0){ return start {offset, true }; }

    str substr (int pos, int num) const {
        if (pos < 0) num += pos, pos = 0;
        num = std::min (num, size() - pos);
        return num > 0 ? base::substr (pos, num) : base ();
    }
    str substr (range range) const { return substr(range.pos, range.size);}
    str substr (start start) const { return start.from_end ? till(size()-start.offset) : from(start.offset); }

    str from (int pos) const { return substr (pos, max); }
    str till (int pos) const { return substr (0, pos); }
    str head (int num) const { return substr (0, num); }
    str tail (int num) const { return substr (size () - num, max); }

    bool starts_with (str s) const {
        int n = s.size(); if (size() < n) return false;
        auto i = begin(); auto j = s.begin();
        while (n--) if (*i++ != *j++) return false;
        return true;
    }
    bool ends_with   (str s) const {
        int n = s.size(); if (size() < n) return false;
        auto i = rbegin(); auto j = s.rbegin();
        while (n--) if (*i++ != *j++) return false;
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////

    struct one_of     { base chars; one_of    (str s = "") : chars(s) {}};
    struct one_not_of { base chars; one_not_of(str s = "") : chars(s) {}};

    typedef std::variant<str, one_of, one_not_of> pattern;

    static int size_of(pattern pattern) { return
        std::holds_alternative<str>(pattern) ? std::get<0>(pattern).size() :
        std::holds_alternative<one_of>(pattern) ? (std::get<1>(pattern).chars.size() == 0 ? 0 : 1) :
        std::holds_alternative<one_not_of>(pattern) ? (std::get<2>(pattern).chars.size() == 0 ? 0 : 1) : 0;
    }

    auto find (pattern pattern, start start = start_from(0)) const
    {
        const_iterator i;

        if (size_of(pattern) == 0) throw std::logic_error("str::find : empty pattern");

        if (std::holds_alternative<str>(pattern))
        {
            if (start.from_end) { auto it = 
                std::search(rbegin() + start.offset, rend(),
                std::boyer_moore_horspool_searcher(
                std::get<str>(pattern).rbegin(),
                std::get<str>(pattern).rend ()));
                i = it == rend() ? end() : std::next(it).base();
            }

            else i = 
                std::search(begin() + start.offset, end(),
                std::boyer_moore_horspool_searcher(
                std::get<str>(pattern).begin(),
                std::get<str>(pattern).end ()));
        }

        else

        if (std::holds_alternative<one_of>(pattern))
        {
            auto n = start.from_end ?
                find_last_of (std::get<one_of>(pattern).chars, size() - 1 - start.offset):
                find_first_of(std::get<one_of>(pattern).chars, start.offset);
            i = n == npos ? end() : begin() + n;
        }

        else

        if (std::holds_alternative<one_not_of>(pattern))
        {
            auto n = start.from_end ?
                find_last_not_of (std::get<one_not_of>(pattern).chars, size() - 1 - start.offset):
                find_first_not_of(std::get<one_not_of>(pattern).chars, start.offset);
            i = n == npos ? end() : begin() + n;
        }

        return range {(int)(i - begin()), i == end() ? 0 : size_of(pattern)};
    }

    bool contains (pattern pattern, start start = start_from(0)) const { return find (pattern, start).size > 0; }

    bool contains_only (pattern pattern, start start = start_from(0)) const { return
        std::holds_alternative<str>(pattern) ?    substr(start) == std::get<0>(pattern) :
        std::holds_alternative<one_of>(pattern) ? find (one_not_of(std::get<1>(pattern).chars), start).empty() :
        std::holds_alternative<one_not_of>(pattern) ? find (one_of(std::get<2>(pattern).chars), start).empty() : false;
    }

    /////////////////////////////////////////////////

    enum class delimiter { exclude, to_the_left, to_the_right };

    bool split_by (pattern pattern, start start, str& str1, str& str2, delimiter delimiter = delimiter::exclude) const
    {
        auto range = find (pattern, start); int n = range.pos; int m = range.size;
        str1 = m > 0 ? head (n     + (delimiter == delimiter::to_the_left  ? m : 0)) : *this;
        str2 = m > 0 ? from (n + m - (delimiter == delimiter::to_the_right ? m : 0)) : str{};
        return m > 0;
    }
    bool split_by (pattern pattern, str& str1, str& str2, delimiter delimiter = delimiter::exclude) const
    {
        return split_by (pattern, start_from(0), str1, str2, delimiter);
    }
    array<str> split_by (pattern pattern) const
    {
        array<str> result;
        int start = 0; while (true) {
            auto range = find(pattern, start_from(start));
            bool found = range.size > 0;
            result += substr(start, found ? range.pos - start : size() - start);
            start = range.pos + range.size;
            if (start >= size()) { if (found) result += ""; break; }
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////

#if 0

    void overwrite (int pos,          str s){ erase (pos, s.size()); insert (pos, s); }
    void replace   (int pos, int num, str s){ erase (pos, num     ); insert (pos, s); }
    void erase     (int pos, int num = 1   ){ if (pos < 0) num += pos, pos = 0; num = std::min (num, size() - pos); if (num > 0) base::erase (pos, num); }
    void erase     (range range) { erase(range.pos, range.size);}
    void truncate  (int pos                ){ erase (pos, max); }
    void truncate  (                       ){ erase (size() - 1); }
    void strip     (const str & chars = " "){ trimr (chars); triml (chars); }
    void triml     (const str & chars = " "){ int n = find (one_not_of {chars});                    if (n == nope) clear(); else erase (0, n); }
    void trimr     (const str & chars = " "){ int n = find (one_not_of {chars}, start_from_end ()); if (n == nope) clear(); else truncate (n+1); }

    int replace_all (str from, str to){ int pos = 0, nn = 0; while ((pos = find (from, start_from(pos))) != nope){ replace (pos, from.size (), to); pos += to.size (); nn++; }; return nn; }

    void canonicalize ()
    {
        for (char & c : *this) if( c == '\t' || c  == '\r' || c == '\n' )  c = ' ';
        base::erase(std::unique(begin(), end(), [](char c1, char c2){ return c1 == ' ' && c2 == ' '; }), end());
        strip();
    }
#endif

    bool ascii_isalnum () const {
        for (char c : *this)
            if (c < '0' || c > '9' &&
                c < 'A' || c > 'Z' &&
                c < 'a' || c > 'z') return false; return true; }

    static char ascii_tolower (char c) { return c >= 'A' || c <= 'Z' ? c - 'A' + 'a' : c; }
    static char ascii_toupper (char c) { return c >= 'a' || c <= 'z' ? c - 'a' + 'A' : c; }

    str ascii_lowercased () const { str s = *this; std::transform(s.begin(), s.end(), s.begin(), ascii_tolower); return s; }
    str ascii_uppercased () const { str s = *this; std::transform(s.begin(), s.end(), s.begin(), ascii_toupper); return s; }
};

namespace std
{
    template <> struct hash<str>
    {
        std::size_t operator()(const str& s) const
        {
            using std::size_t;
            using std::hash;
            using std::string;
            return hash<string>()(s);
        }
    };
}
