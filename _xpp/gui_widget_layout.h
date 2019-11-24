#pragma once
#include "gui_widget.h"
#include "gui_widgetarium.h"
namespace gui
{
    enum class orientation { horizontal, vertical };
    const orientation vertical = orientation::vertical;
    const orientation horizontal = orientation::horizontal;
    constexpr orientation operator ~ (orientation orientation)
    { return orientation == horizontal ? vertical : horizontal; }

    template<class T, orientation> struct layout;

    template<class T> struct layout<T, horizontal> : widgetarium<T>
    {
        typedef widgetarium<T> base;
        using base::size; using base::coord;

        template<class... Args> T & emplace_back (Args&&... args)
        {
            int x = offset(size()); // before base::emplace_back
            T & t = base::emplace_back(std::forward<Args>(args)...);
            int y = base::baseline.y - t.baseline.y;
            t.move_to(XY(x,y));
            base::resize(XY(x + t.coord.now.size.x, coord.now.size.y));
            if (y < 0) {
                y = -y;
                for (auto & w : *this) w.shift(XY(0,y));
                int descent = max (t.coord.now.size.y - t.baseline.y,
                    coord.now.size.y - base::baseline.y);
                base::baseline.y += y;
                base::resize(XY(coord.now.size.x, base::baseline.y + descent));
            }
            return t;
        }

        int offset (int n) const {
            assert(n >= 0);
            assert(n <= size());
            return n <  size() ? (*this)(n).coord.now.x :
                   n >  0 ? (*this)(n-1).coord.now.x + (*this)(n-1).coord.now.size.x + (*this)(n-1).advance.x :
                   0;
        }

        int rotate (int f, int m, int l)
        {
            assert(f <= m); assert(f >= 0);
            assert(m <= l); assert(l <= size());
            if(f == m) return l;
            if(m == l) return f;
            int d1 = offset(m) - offset(f);
            int d2 = offset(l) - offset(m);
            int n = base::rotate(f, m, l);
            for (int i=f; i<n; i++) (*this)(i).shift(XY(-d1,0));
            for (int i=n; i<l; i++) (*this)(i).shift(XY( d2,0));
            return n;
        }

        void erase (int pos, int num = 1)
        {
            assert(pos >= 0);
            assert(pos+num <= size());
            pos = rotate(pos, pos+num, size());
            while (size() > pos) base::erase(size()-1);
            base::resize(XY(offset(size()), coord.now.size.y));
            int y = 0;
            int h = 0;
            for (auto & w : *this) {
                y = max (y, w.baseline.y);
                h = max (h, w.coord.now.y + w.coord.now.size.y);
            }
            base::resize(XY(coord.now.size.x, h));
            base::baseline.y = y;
        }
        void truncate (int pos) { while (size() > pos) erase(size()-1); }
        void clear () { truncate (0); }
    };

    template<class T> struct layout<T, vertical> : widgetarium<T>
    {
        typedef widgetarium<T> base;
        using base::size; using base::coord;

        template<class... Args> T & emplace_back (Args&&... args)
        {
            int y = offset(size()); // before base::emplace_back
            T & t = base::emplace_back(std::forward<Args>(args)...);
            int x = base::baseline.x - t.baseline.x;
            t.move_to(XY(x,y));
            base::resize(XY(coord.now.size.x, y + t.coord.now.size.y));
            if (x < 0) {
                x = -x;
                for (auto & w : *this) w.shift(XY(x,0));
                int descent = max (t.coord.now.size.x - t.baseline.x,
                    coord.now.size.x - base::baseline.x);
                base::baseline.x += x;
                base::resize(XY(base::baseline.x + descent, coord.now.size.y));
            }
            return t;
        }

        int offset (int n) const {
            assert(n >= 0);
            assert(n <= size());
            return n <  size() ? (*this)(n).coord.now.y :
                   n >  0 ? (*this)(n-1).coord.now.y + (*this)(n-1).coord.now.size.y + (*this)(n-1).advance.y :
                   0;
        }

        int rotate (int f, int m, int l)
        {
            assert(f <= m); assert(f >= 0);
            assert(m <= l); assert(l <= size());
            if(f == m) return l;
            if(m == l) return f;
            int d1 = offset(m) - offset(f);
            int d2 = offset(l) - offset(m);
            int n = base::rotate(f, m, l);
            for (int i=f; i<n; i++) (*this)(i).shift(XY(0,-d1));
            for (int i=n; i<l; i++) (*this)(i).shift(XY(0, d2));
            return n;
        }

        void erase (int pos, int num = 1)
        {
            assert(pos >= 0);
            assert(pos+num <= size());
            pos = rotate(pos, pos+num, size());
            while (size() > pos) base::erase(size()-1);
            base::resize(XY(coord.now.size.x, offset(size())));
            int x = 0;
            int w = 0;
            for (auto & t : *this) {
                x = max (x, t.baseline.x);
                w = max (w, t.coord.now.x + t.coord.now.size.x);
            }
            base::resize(XY(w, coord.now.size.y));
            base::baseline.x = x;
        }
        void truncate (int pos) { while (size() > pos) erase(size()-1); }
        void clear () { truncate (0); }
    };

    template<class T, orientation orientation = vertical>
    struct page : layout<layout<T,~orientation>, orientation>
    {
    };
}
