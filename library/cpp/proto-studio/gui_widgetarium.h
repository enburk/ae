#pragma once
#include <any>
#include <set>
#include "gui_widget.h"
namespace gui
{
    template<class T> struct widgetarium : widget<widgetarium<T>>
    {
        using widget = widget<widgetarium<T>>;
        using widget::children;

        array<int> holes;
        array<int> indices;
        std::deque<std::optional<T>> deque;

        int size () const { return indices.size(); }

        void reserve (int n)
        {
            holes.reserve(n);
            indices.reserve(n);
            children.reserve(n);
        }

        void on_notify (base::widget* w) override {
            for (int i = 0; i < size(); i++)
                if (&(*this)(i) == w)
                    widget::notify(i);
        }

        const T & operator () (int pos) const {
            if (pos >= size()) throw std::out_of_range
                ("widgetarium size = " + std::to_string(size())
                    + ", accessed at " + std::to_string(pos));
            return *deque[indices[pos]];
        }
        /***/ T & operator () (int pos) /***/ {
            if (pos > size()) throw std::out_of_range
                ("widgetarium size = " + std::to_string(size())
                    + ", accessed at " + std::to_string(pos));
            if (pos == size()) emplace_back();
            return *deque[indices[pos]];
        }

        const T & back () const { return *deque[indices.back()]; }
        /***/ T & back () /***/ { return *deque[indices.back()]; }

        template<class... Args>
        T & emplace_back (Args&&... args)
        {
            if (holes.empty()) { auto & t =
                deque.emplace_back(std::in_place, std::forward<Args>(args)...);
                indices.emplace_back(size());
                children += &t.value();
                t.value().parent = this;
                return t.value();
            } else {
                int index = holes.back(); holes.pop_back(); auto & t =
                deque[index].emplace(std::forward<Args>(args)...);
                indices.emplace_back(index);
                children += &t;
                t.parent = this;
                return t;
            }
        }

        int rotate (int f, int m, int l) {
            assert(children.size() == indices.size());
            std::rotate(
            children.begin() + f,
            children.begin() + m,
            children.begin() + l);
            return (int)(
            std::rotate(
            indices.begin() + f,
            indices.begin() + m,
            indices.begin() + l) -
            indices.begin());
        }

        void erase (int pos)
        {
            deque[indices[pos]].reset();
            holes.push_back(indices[pos]);
            indices.erase(indices.begin()+pos);
        }
        void truncate (int pos) { while (size() > pos) erase(size()-1); }
        void clear () { truncate (0); }

        template<class U> struct iterator
        {
            widgetarium<U>* that; int index;
            void operator ++ () { ++index; }
            void operator -- () { --index; }
            void operator += (int n) { index += n; }
            void operator -= (int n) { index -= n; }
            bool operator == (iterator i) { return index == i.index; }
            bool operator != (iterator i) { return index != i.index; }
            const U & operator * () const { return (*that)(index); }
            /***/ U & operator * () /***/ { return (*that)(index); }
            friend iterator operator + (iterator i, int n) { i.index += n; return i; }
            friend iterator operator - (iterator i, int n) { i.index -= n; return i; }
            friend int operator - (iterator i, iterator j) { return i.index - j.index; }
        };
        iterator<T> begin () { return iterator<T>{this, 0}; }
        iterator<T> end   () { return iterator<T>{this, size()}; }
        const iterator<T> begin () const { return iterator<T>{const_cast<widgetarium*>(this), 0}; }
        const iterator<T> end   () const { return iterator<T>{const_cast<widgetarium*>(this), size()}; }
    };
}
