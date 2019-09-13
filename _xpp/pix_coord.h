#pragma once
#include <cmath>
#include <algorithm>
#include "aux_array.h"
namespace pix
{
    using real = double;
    using std::abs;
    using std::min;
    using std::max;
    struct XYWH;
    struct XYXY;
    struct XY
    {
        int x, y;

        XY (            ) : x (0), y (0) {}
        XY (int x, int y) : x (x), y (y) {}

        void operator += (XY r) { x += r.x; y += r.y; }; friend XY operator + (XY a, XY b) { a += b; return a; }
        void operator -= (XY r) { x -= r.x; y -= r.y; }; friend XY operator - (XY a, XY b) { a -= b; return a; }

        void operator *= (int n) { x *= n; y *= n; }; friend XY operator * (XY a, int n) { a *= n; return a; }
        void operator /= (int n) { x /= n; y /= n; }; friend XY operator / (XY a, int n) { a /= n; return a; }

        bool operator == (XY r) { return x == r.x && y == r.y; }
        bool operator != (XY r) { return ! (*this == r); }
    };
    struct XYXY
    {
        union  {
        struct { int xl, yl, xh, yh; };
        struct { int x1, y1, x2, y2; };
        struct { int l, t, r, b; };  };

        XYXY (                          ) : l (0), t (0), r (0), b (0) {}
        XYXY (int l, int t, int r, int b) : l (l), t (t), r (r), b (b) {}
        XYXY (XYWH);

        explicit operator bool () { return r > l && b > t; }

        void operator += (XYXY q) { l += q.l; t += q.t; r += q.r; b += q.b; }; friend XYXY operator + (XYXY a, XYXY q) { a += q; return a; }
        void operator -= (XYXY q) { l -= q.l; t -= q.t; r -= q.r; b -= q.b; }; friend XYXY operator - (XYXY a, XYXY q) { a -= q; return a; }

        friend XYXY operator & (XYXY a, XYXY q) {
            if (!a || !q) return XYXY();
            a.l = max(a.l, q.l); a.t = max(a.t, q.t);
            a.r = min(a.r, q.r); a.b = min(a.b, q.b);
            return a ? a : XYXY();
        }
        friend XYXY operator | (XYXY a, XYXY q) {
            if (!q) return a; if (!a) return q;
            a.l = min(a.l, q.l); a.t = min(a.t, q.t);
            a.r = max(a.r, q.r); a.b = max(a.b, q.b);
            return a;
        }
        void operator &= (XYXY q) { *this = *this & q; };
        void operator |= (XYXY q) { *this = *this | q; };

        void operator += (XY p) { l += p.x; t += p.y; r += p.x; b += p.y; }; friend XYXY operator + (XYXY a, XY p) { a += p; return a; }
        void operator -= (XY p) { l -= p.x; t -= p.y; r -= p.x; b -= p.y; }; friend XYXY operator - (XYXY a, XY p) { a -= p; return a; }

        void operator *= (int n) { l *= n; t *= n; r *= n; b *= n; }; friend XYXY operator * (XYXY a, int n) { a *= n; return a; }
        void operator /= (int n) { l /= n; t /= n; r /= n; b /= n; }; friend XYXY operator / (XYXY a, int n) { a /= n; return a; }

        bool operator == (XYXY q) const { return l == q.l && t == q.t && r == q.r && b == q.b; }
        bool operator != (XYXY q) const { return ! (*this == q); }

        bool includes (XY p) const { return l <= p.x && p.x <  r && t <= p.y && p.y <  b; }
        bool excludes (XY p) const { return !includes(p); }

        void inflate (int n) { l -= n; t -= n; r += n; b += n; }
        void deflate (int n) { l += n; t += n; r -= n; b -= n; }

        XY origin () const { return XY (l,t); }
        XY size   () const { return XY (r-l,b-t); }
    };
    struct XYWH
    {
        int x, y, w, h;

        XYWH (                          ) : x (0), y (0), w (0), h (0) {}
        XYWH (int x, int y, int w, int h) : x (x), y (y), w (w), h (h) {}
        XYWH (XYXY);

        explicit operator bool () { return w > 0 && h > 0; }

        void operator += (XYWH r) { x += r.x; y += r.y; w += r.w; h += r.h; }; friend XYWH operator + (XYWH a, XYWH b) { a += b; return a; }
        void operator -= (XYWH r) { x -= r.x; y -= r.y; w -= r.w; h -= r.h; }; friend XYWH operator - (XYWH a, XYWH b) { a -= b; return a; }

        void operator &= (XYWH r) { *this = XYXY(*this) & XYXY(r); }; friend XYWH operator & (XYWH a, XYWH b) { a &= b; return a; }
        void operator |= (XYWH r) { *this = XYXY(*this) | XYXY(r); }; friend XYWH operator | (XYWH a, XYWH b) { a |= b; return a; }

        void operator += (XY p) { x += p.x; y += p.y; }; friend XYWH operator + (XYWH r, XY p) { r += p; return r; }
        void operator -= (XY p) { x -= p.x; y -= p.y; }; friend XYWH operator - (XYWH r, XY p) { r -= p; return r; }

        void operator *= (int n) { x *= n; y *= n; w *= n; h *= n; }; friend XYWH operator * (XYWH r, int n) { r *= n; return r; }
        void operator /= (int n) { x /= n; y /= n; w /= n; h /= n; }; friend XYWH operator / (XYWH r, int n) { r /= n; return r; }

        bool operator == (XYWH r) const { return x == r.x && y == r.y && w == r.w && h == r.h; }
        bool operator != (XYWH r) const { return ! (*this == r); }

        bool includes (XY p) const { return x <= p.x && p.x <  x + w && y <= p.y && p.y <  y + h; }
        bool excludes (XY p) const { return !includes(p); }

        void inflate (int n) { x -= n; y -= n; w += n+n; h += n+n; }
        void deflate (int n) { x += n; y += n; w -= n+n; h -= n+n; }

        XY origin () const { return XY (x,y); }
        XY size   () const { return XY (w,h); }
    };

    inline XYXY::XYXY(XYWH q) : l (q.x), t (q.y), r (q.x+q.w), b (q.y+q.h) {}
    inline XYWH::XYWH(XYXY q) : x (q.l), y (q.t), w (q.r-q.l), h (q.b-q.t) {}

    inline XYXY operator & (XYXY a, XYWH b) { a &= XYXY(b); return a; }
    inline XYXY operator | (XYXY a, XYWH b) { a |= XYXY(b); return a; }
    inline XYXY operator & (XYWH a, XYXY b) { b &= XYXY(a); return b; }
    inline XYXY operator | (XYWH a, XYXY b) { b |= XYXY(a); return b; }

    struct rectifier
    {
        array<XYWH> rr;
        void operator  = (XYWH r) { rr.clear(); rr += r; }
        void operator += (XYWH r) {
            if (!r) return;
            //for (auto & R : rr) {
            //    XYWH u = R | r;
            //    auto Rsquare = R.size.x * R.size.y;
            //    auto rsquare = r.size.x * r.size.y;
            //    auto usquare = u.size.x * u.size.y;
            //    if  (usquare > Rsquare + rsquare) continue;
            //    std::swap(R, rr.back());
            //    rr.pop_back();
            //    *this += u;
            //    return;
            //}
            rr += r;
        }
        typedef array<XYWH>::iterator iterator;
        iterator begin () { return rr.begin(); }
        iterator end   () { return rr.end(); }
        void     clear () { rr.clear(); }
    };
}