#pragma once
#include <vector>
#include "pix_coord.h"

template<typename T> struct FRAME
{
    typedef T type; type* p; XY size; int pitch;

    FRAME (                           ) : p (nullptr), pitch (0) {}
    FRAME (type* p, XY size           ) : p (p), size (size), pitch (size.x) {}
    FRAME (type* p, XY size, int pitch) : p (p), size (size), pitch (pitch ) {}
 
    type  at (int x, int y) const && { return *(p + y*pitch + x); }
    type& at (int x, int y) &        { return *(p + y*pitch + x); }

    type  operator () (int x, int y) const && { return at(x,y); }
    type& operator () (int x, int y) &        { return at(x,y); }

    bool  operator == (const FRAME & e) const { return p == e.p && size == e.size && pitch == e.pitch; }
    bool  operator != (const FRAME & e) const { return p != e.p || size != e.size || pitch != e.pitch; }

    FRAME frame (XYWH r) const { return FRAME (p + r.y*pitch + r.x, XY (r.w, r.h), pitch); }

    explicit operator XYWH () const { return XYWH (0, 0, size.x, size.y); }

    void  fill (const type & c) {
        for (int y=0; y<size.y; y++)
        for( int x=0; x<size.x; x++) at(x,y) = c;
    }
};

template<typename T> struct IMAGE
{
    typedef T type; XY size; std::vector<type> data;

	explicit IMAGE (               ) : size (0, 0), data () {}
	explicit IMAGE (XY size        ) : size (size), data (size.x * size.y) {}
	explicit IMAGE (XY size, type c) : size (size), data (size.x * size.y, c) {}

    template<typename U> explicit IMAGE (FRAME<U> f) : size (f.size), data (f.size.x * f.size.y)
    {
        for (int y=0; y<size.y; y++)
        for (int x=0; x<size.x; x++) at(x,y) = type (f.at(x,y));
    }
 
    type  at (int x, int y) const && { return data[y*size.x + x]; }
    type& at (int x, int y) &        { return data[y*size.x + x]; }

    type  operator () (int x, int y) const && { return at(x,y); }
    type& operator () (int x, int y) &        { return at(x,y); }

    FRAME<type> frame (XYWH r) const { return FRAME<type>(data.data(), size, size.x).frame(r); }

    explicit operator FRAME<type> () const { return FRAME<type>(data.data(), size, size.x); }

    explicit operator XYWH () const { return XYWH (0, 0, size.x, size.y); }

	void resize (XY Size) { data.resize (Size.x * Size.y); size = Size; }

    void fill (const type & c) { for (auto & v : data) v = c; }
};
