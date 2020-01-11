#pragma once
#include <vector>
#include "aux_abc.hpp"
namespace aux
{
    template<class T, class C = std::vector<T>> struct array : C
    {
        typedef T type; typedef C base;
        using typename base::const_iterator;
        using typename base::iterator;
        using base::begin;
        using base::end;
        using base::rbegin;
        using base::rend;
        
        int  size    () const { return (int) base::size(); }
        void resize  (int n)  { base::resize (max(0,n)); }
        void reserve (int n)  { base::reserve(max(0,n)); }

        array (              ) = default;
        array (const array  &) = default;
        array (      array &&) = default;
        array (const base  &c) : base(c) {}
        array (      base &&c) : base(std::move(c)) {}
        array (std::initializer_list<T> list) : base(list) {}

        auto& operator =  (const array  & a) { base::operator = (a); return *this; }
        auto& operator =  (      array && a) { base::operator = (std::move(a)); return *this; }

        void  operator += (const type   & e) { base::push_back (e); }
        void  operator += (      type  && e) { base::push_back (std::move(e)); }

        void  operator += (const array  & a) { base::insert (end(), a.begin(), a.end()); }
        void  operator += (      array && a) { base::insert (end(),
            std::make_move_iterator(a.begin()),
            std::make_move_iterator(a.end())); }

        friend array operator + (const array & a, const type  & b) { array r = a; r += b; return r; }
        friend array operator + (const type  & a, const array & b) { array r = a; r += b; return r; }

        ////////////////////////////////////////////////////////////////////////

        struct range
        {
            array* arr; int offset, length;

            struct iterator
            {
                type *begin, *current;
                void operator ++ () { ++current; }
                void operator -- () { --current; }
                void operator += (int n) { current += n; }
                void operator -= (int n) { current -= n; }
                bool operator == (iterator i) { return current == i.current; }
                bool operator != (iterator i) { return current != i.current; }
                std::pair<type&, int> operator -> () { return **this; }
                std::pair<type&, int> operator *  () { return std::pair<type&, int>{*current,(int)(current-begin)}; }
                std::pair<const type&, int> operator -> () const { return **this; }
                std::pair<const type&, int> operator *  () const { return std::pair<const type&, int>{*current,(int)(current-begin)}; }
                friend iterator operator + (iterator i, int n) { i.current += n; return i; }
                friend iterator operator - (iterator i, int n) { i.current -= n; return i; }
            };
            iterator begin () { auto p = arr->data(); return iterator{p+offset, p+offset}; }
            iterator end   () { auto p = arr->data(); return iterator{p+offset, p+offset+length}; }
            const iterator begin () const { auto p = arr->data(); return iterator{p+offset, p+offset}; }
            const iterator end   () const { auto p = arr->data(); return iterator{p+offset, p+offset+length}; }

            explicit
            operator bool () const { return length > 0; }
            bool empty () const { return length == 0; }

            range size (int n) const
            {
                n = max(n, 0);
                n = min(n, length);
                return range{arr, offset, n};
            }

            range upto (int i) const
            {
                i = max(i, offset);
                i = min(i, arr->size());
                return range{arr, offset, i-offset};
            }

            void erase ()
            {
                arr->erase(
                arr->begin()+offset,
                arr->begin()+offset+length);
                length = 0;
            }

            void replace_by (array a) { replace_by(a.whole()); }
            void replace_by (range r)
            {
                int n = min(length, r.length);
                for (int i=0; i<n; i++) (*arr)[offset+i] = (*r.arr)[r.offset+i];
                if (length > r.length) arr->from(offset+n).upto(offset+length).erase();
                if (length < r.length) { r.offset += n; r.length -= n; arr->insert(offset+n, r); r.length += n; }
                    length = r.length;
            }

            friend bool operator != (const range & l, const range & r) { return !(l == r); }
            friend bool operator == (const range & l, const range & r) {
                return l.length == r.length && std::equal(
                    l.arr->begin() + l.offset,
                    l.arr->begin() + l.offset + l.length,
                    r.arr->begin() + r.offset);
            }

            friend bool operator == (const range & l, const array & r) { return l == r.whole(); }
            friend bool operator != (const range & l, const array & r) { return l != r.whole(); }
            friend bool operator == (const array & l, const range & r) { return l.whole() == r; }
            friend bool operator != (const array & l, const range & r) { return l.whole() != r; }
        };

        array (range r) : base(
            r.arr->begin() + r.offset,
            r.arr->begin() + r.offset + r.length) {}

        range whole     () { return range{this, 0, size()}; }
        range from (int i) { i = min(i, size()); return range{this, i, size()-i}; }
        range upto (int i) { i = min(i, size()); return range{this, 0, i}; }

        const range whole     () const { return range{const_cast<array*>(this), 0, size()}; }
        const range from (int i) const { i = min(i, size()); return range{const_cast<array*>(this), i, size()-i}; }
        const range upto (int i) const { i = min(i, size()); return range{const_cast<array*>(this), 0, i}; }

        ////////////////////////////////////////////////////////////////////////

        auto find         (const type & e) const { return std::find (begin(), end(), e); }
        bool contains     (const type & e) const { return std::find (begin(), end(), e) != end(); }
        bool binary_found (const type & e) const { return std::binary_search (begin(), end(), e); }

        template<class Compare>
        range equal_range (const type & e, Compare compare = std::less<>{}) const
        {
            auto r = std::equal_range (begin(), end(), e, compare);
            return range{this, (int)(r.first-begin()), (int)(r.second()-r.first())};
        }

        ////////////////////////////////////////////////////////////////////////

        template<class F> void sort (F compare = std::less<>{}) { std::sort(begin(), end(), compare); }

        template<class P>
        void erase_if (P predicate) { base::erase(std::remove_if(begin(), end(), predicate), end()); }
        auto erase (const_iterator f, const_iterator l) { return base::erase(f, l); }
        auto erase (const_iterator i                  ) { return base::erase(i); }
        auto erase (int            i                  ) { return base::erase(begin()+i); }

        void truncate  (int offset) { from(offset).erase(); }
        void truncate  () { if (size() > 0) base::pop_back(); }

        template<class I>
        auto insert(const_iterator i, I f, I l) { return base::insert(i, f, l); }
        auto insert(int i, const array & a    ) { return insert(i, a.whole()); }
        auto insert(int i, const range & r    ) { return insert(
            this ->begin() + i,
            r.arr->begin() + r.offset,
            r.arr->begin() + r.offset + r.length);
        }

        void try_erase       (const type & e) { auto it = find(e); if (it != end()) erase(it); }
        void try_emplace     (const type & e) { auto it = find(e); if (it == end()) *this += e; }
        auto find_or_emplace (const type & e)
        {
            auto it = find(e); if (it != end()) return it;
            *this += e; it = end()-1; return it;
        }
    };
}
