#pragma once
#include <cmath>
#include <algorithm>
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

struct XYWH
{
    int x, y, w, h;

    XYWH (                          ) : x (0), y (0), w (0), h (0) {}
    XYWH (int x, int y, int w, int h) : x (x), y (y), w (w), h (h) {}
    XYWH (XYXY);

    void operator += (XYWH r) { x += r.x; y += r.y; w += r.w; h += r.h; }; friend XYWH operator + (XYWH a, XYWH b) { a += b; return a; }
    void operator -= (XYWH r) { x -= r.x; y -= r.y; w -= r.w; h -= r.h; }; friend XYWH operator - (XYWH a, XYWH b) { a -= b; return a; }

    void operator *= (int n) { x *= n; y *= n; w *= n; h *= n; }; friend XYWH operator * (XYWH a, int n) { a *= n; return a; }
    void operator /= (int n) { x /= n; y /= n; w /= n; h /= n; }; friend XYWH operator / (XYWH a, int n) { a /= n; return a; }

    bool operator == (XYWH r) { return x == r.x && y == r.y && w == r.x && h == r.y; }
    bool operator != (XYWH r) { return ! (*this == r); }

    bool include (XY p) { return x <= p.x && p.x <  x + w && y <= p.y && p.y <  y + h; }
    bool exclude (XY p) { return x >  p.x || p.x >= x + w || y >  p.y || p.y >= y + h; }
};

struct XYXY
{
    union  {
    struct { int xl, yl, xh, yh; };
    struct { int l, t, r, b; };  };

    XYXY (                          ) : l (0), t (0), r (0), b (0) {}
    XYXY (int l, int t, int r, int b) : l (l), t (t), r (r), b (b) {}
    XYXY (XYWH);

    void operator += (XYXY q) { l += q.l; t += q.t; r += q.r; b += q.b; }; friend XYXY operator + (XYXY a, XYXY q) { a += q; return a; }
    void operator -= (XYXY q) { l -= q.l; t -= q.t; r -= q.r; b -= q.b; }; friend XYXY operator - (XYXY a, XYXY q) { a -= q; return a; }

    void operator *= (int n) { l *= n; t *= n; r *= n; b *= n; }; friend XYXY operator * (XYXY a, int n) { a *= n; return a; }
    void operator /= (int n) { l /= n; t /= n; r /= n; b /= n; }; friend XYXY operator / (XYXY a, int n) { a /= n; return a; }

    bool operator == (XYXY q) { return l == q.l && t == q.t && r == q.r && b == q.b; }
    bool operator != (XYXY q) { return ! (*this == q); }

    bool include (XY p) { return l <= p.x && p.x <  r && t <= p.y && p.y <  b; }
    bool exclude (XY p) { return l >  p.x || p.x >= r || t >  p.y || p.y >= b; }
};

inline XYXY::XYXY(XYWH q) : l (q.x), t (q.y), r (q.x+q.w), b (q.y+q.h) {}
inline XYWH::XYWH(XYXY q) : x (q.l), y (q.t), w (q.r-q.l), h (q.b-q.t) {}
