#pragma once
#include "gui_widget.h"
namespace gui
{
    enum class orientation { horizontal, vertical };
    const orientation vertical = orientation::vertical;
    const orientation horizontal = orientation::horizontal;
    constexpr orientation operator ~ (orientation o) {
    return o == horizontal ? vertical : horizontal;
    }

    template<class T, orientation orientation> struct layout : widgetarium<T>
    {
        typedef widgetarium<T> base;

        template<class... Args> T & emplace_back (Args&&... args)
        {
            int x = advance(0, size());
            T & t = base::emplace_back(std::forward<Args>(args)...);
            shift(size()-1, size(), XY(x, baseline - t.baseline));
            if (baseline < t.baseline) { shift(0, size(), XY(0,
               -baseline + t.baseline));
                baseline = t.baseline; }
            x += length(size()-1);
            resize(XY(x, max(coord.now.size.y, g.size.y)));
            resize(XY(x, max(coord.now.size.y, g.size.y)));


            return t;
        }

        void erase (int pos)
        {
            deque[indices[pos]].reset();
            holes.push_back(indices[pos]);
            indices.erase(indices.begin()+pos);
        }

        void truncate (int pos)
        {
            while (indices.size() > pos)
                erase(indices.size()-1);
        }

        XY coord (int d) { return orientation == horizontal ? (*this)(n).coord.now.x : (*this)(n).coord.now.y; }

        int offset  (int n) { return orientation == horizontal ? (*this)(n).coord.now.x : (*this)(n).coord.now.y; }
        int length  (int n) { return orientation == horizontal ? (*this)(n).coord.now.w : (*this)(n).coord.now.h; }
        int breadth (int n) { return orientation == horizontal ? (*this)(n).coord.now.h : (*this)(n).coord.now.w; }
        int advance (int n) { return orientation == horizontal ? (*this)(n).advance  .x : (*this)(n).advance  .y; }

        int lenght  (int f, int l) { --l; return f > l ? 0 : offset(l) - offset(f) + length (l); }
        int advance (int f, int l) { --l; return f > l ? 0 : offset(l) - offset(f) + advance(l); }

        void shift (int f, int l, XY d) {
            for (int i=f; i<l; i++)
                (*this)(i).shift(orientation == horizontal ?
                    d : XY(d.y, d.x));
        }

        int rotate (int f, int m, int l)
        {
            int a1 = advance(f,m);
            int a2 = advance(m,l);
            int n = base::rotate(f, m, l);
            shift(f,n,-a1);
            shift(n,l, a2);
        }





        struct line final : widget<line>
        {
            widgetarium<canvas> canvases;
            widgetarium<glyph> glyphs;
            int minimum_height = 0;
            int baseline = 0;

            void append (sys::glyph g)
            {
                int x = advance(0, glyphs.size());
                auto & glyph = glyphs.emplace_back();
                glyph.g = g;
                glyph.shift(XY(x, baseline - g.ascent));
                if (baseline < g.ascent) { shift(0, glyphs.size(), XY(0,
                   -baseline + g.ascent));
                    baseline = g.ascent; }
                x += g.size.x;
                glyphs.
                resize(XY(x, max(coord.now.size.y, g.size.y)));
                resize(XY(x, max(coord.now.size.y, g.size.y)));
            }
            void append (sys::token token)
            {
                for (auto & g : token.glyphs)
                    append (g);
            }
            void insert (int pos, sys::glyph g) {
                append(std::move(g)); rotate(pos,
                    glyphs.size()-1,
                    glyphs.size());
            }
            void insert (int pos, sys::token token) {
                append(std::move(token)); rotate(pos,
                    glyphs.size()-token.glyphs.size(),
                    glyphs.size());
            }
            void erase (int pos, int num = 1)
            {
                glyphs.truncate(rotate(pos, pos+num, glyphs.size()));
                int height = baseline = 0;
                for (auto & g : glyphs) {
                    baseline = max (baseline, g.g.now.ascent);
                    height =   max (height,   g.g.now.size.y);
                }
                height = max (height, minimum_height);
                for (auto & g : glyphs) g.move_to(XY(g.coord.now.x, baseline - g.g.now.ascent));
                glyphs.
                resize(XY(glyphs.coord.now.size.x, height));
                resize(XY(glyphs.coord.now.size.x, height));
            }
        };
    };

    template<class T, orientation orientation = vertical>
    struct page : layout<layout<T,~orientation>, orientation>
    {
    };
}
