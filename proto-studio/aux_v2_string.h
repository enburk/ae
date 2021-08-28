#pragma once
#include <string>
#include "aux_v2_ranges.h"
#include "aux_v2_collections.h"
#include "aux_v2_array.h"
namespace aux
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
            text.data(), text.data() + text.size()); }

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

        #include "aux_v2_algo_random.h"
        #include "aux_v2_algo_resizing.h"

        void triml (const str & chars = " "){
             upto(first(one_not_of(chars))
                .begin()).erase();
        }
        void trimr (const str & chars = " "){
             from(last(one_not_of(chars))
                .end()).erase();
        }
        void strip (const str & chars = " "){
             trimr(chars);
             triml(chars);
        }
        void truncate () { if (size() > 0)
             container::resize(size()-1);
        }



        enum class delimiter { exclude, to_the_left, to_the_right };

        bool split_by (str pattern, str& str1, str& str2, delimiter delimiter = delimiter::exclude) const
        {
            int n = container::find (pattern); int m = n == container::npos ? 0 : pattern.size();
            str s1 = m > 0 ? str(upto (n     + (delimiter == delimiter::to_the_left  ? m : 0))) : *this;
            str s2 = m > 0 ? str(from (n + m - (delimiter == delimiter::to_the_right ? m : 0))) : str{};
            str1 = std::move(s1); // could be this
            str2 = std::move(s2); // could be this
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

        int replace_all (char c, int ccc, str to) // c repeated exactly ccc times
        {
            int pos = 0, nn = 0;
            while (true) {
                auto range = from(pos).first(str(c, ccc)); if (range.empty()) break;
                pos = range.begin() - begin();
                if (pos+ccc >= size() || at(pos+ccc) != c) {
                    range.replace_by(to);
                    pos += to.size ();
                    nn++;
                }
                else {
                    auto range = from(pos).first(one_not_of{str(c, 1)}); if (range.empty()) break;
                    pos = range.begin() - begin();
                }
            }
            return nn;
        }

        void align_left (int n, char padding = ' ') {
        if (size() < n) *this += str(padding, n - size()); }

        void align_right (int n, char padding = ' ') {
        if (size() < n) *this = str(padding, n - size()) + *this; }

        str left_aligned  (int n, char padding = ' ') {
        str s = *this; s.align_left(n, padding); return s; }

        str right_aligned (int n, char padding = ' ') {
        str s = *this; s.align_right(n, padding); return s; }

    

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

        bool is_ascii_lowercased () const { for (char c : *this) if ('A' <= c and c <= 'Z') return false; return true; }
        bool is_ascii_uppercased () const { for (char c : *this) if ('a' <= c and c <= 'z') return false; return true; }
    };
}
namespace std
{
    template <> struct hash<aux::str>
    {
        std::size_t operator()(const aux::str & s) const
        {
            using std::size_t;
            using std::hash;
            using std::string;
            return hash<string>()(s);
        }
    };
}
one_of     (char*) -> one_of     <aux::str>;
one_not_of (char*) -> one_not_of <aux::str>;
