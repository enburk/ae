#pragma once
#include "pix_coord.h"
namespace pix
{
    template<typename T> struct FRAME;
    template<typename T> struct IMAGE
    {
        typedef T type; std::vector<type> data; XY size; rectifier updates;

	    explicit IMAGE (               ) : data (                  ), size (0, 0) { updates = XYXY(*this); }
	    explicit IMAGE (XY size        ) : data (size.x * size.y   ), size (size) { updates = XYXY(*this); }
	    explicit IMAGE (XY size, type c) : data (size.x * size.y, c), size (size) { updates = XYXY(*this); }

        template<typename U> explicit IMAGE (FRAME<U> f) : IMAGE (f.size)
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = type (f(x,y));
        }
        const
        type& operator () (int x, int y) const { return data[y*size.x + x]; }
        type& operator () (int x, int y)       { return data[y*size.x + x]; }

        explicit operator XYWH () const { return XYWH (0, 0, size.x, size.y); }
        explicit operator XYXY () const { return XYXY (0, 0, size.x, size.y); }

        FRAME<type> frame (      ) { return FRAME<type>(*this); }
        FRAME<type> frame (XYWH r) { return frame().frame(r); }

	    void resize (XY Size) { data.resize (Size.x * Size.y); size = Size; updates = XYXY(*this); }

        void fill (const type & c) { for (auto & v : data) v = c; updates = XYXY(*this); }
    };

    template<typename T> struct FRAME
    {
        typedef T type; IMAGE<type> * image; XY origin, size;

        FRAME (                   ) : image (nullptr) {}
        FRAME (IMAGE<type> & image) : image (&image), size(image.size) {}
        const
        type& operator () (int x, int y) const { return (*image)(origin.x+x, origin.y+y); }
        type& operator () (int x, int y)       { return (*image)(origin.x+x, origin.y+y); }

        bool  operator == (const FRAME & f) const { return image == f.image && origin == f.origin && size == f.size; }
        bool  operator != (const FRAME & f) const { return image != f.image || origin != f.origin || size != f.size; }

        explicit operator XYWH () const { return XYWH (origin.x, origin.y, size.x, size.y); }
        explicit operator XYXY () const { return XYXY (origin.x, origin.y, size.x, size.y); }

        FRAME frame (XYWH r) const {
            r = XYWH(*this) & (r + origin);
            FRAME f;
            f.image  = image;
            f.origin = XY(r.x, r.y);
            f.size   = XY(r.w, r.h);
            return f;
        }

        void  fill (const type & c) {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = c;
            image->updates += XYWH(origin.x, origin.y, size.x, size.y);
        }
        void  blend (const type & c, uint8_t alpha = 255) {
            if (alpha == 0) return;
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y).blend (c, alpha);
            image->updates += XYWH(origin.x, origin.y, size.x, size.y);
        }

        void copy_from (FRAME f) {
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y) = f(x,y);
            image->updates += XYWH(origin.x, origin.y, w, h);
        }
        void copy_to (FRAME f) const { f.copy_from (*this); }

        void blend_from (FRAME f, uint8_t alpha = 255) {
            if (alpha == 0) return;
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y).blend(f(x,y), alpha);
            image->updates += XYWH(origin.x, origin.y, w, h);
        }
        void blend_to (FRAME f, uint8_t alpha = 255) const { f.blend_from (*this, alpha); }

        void copy_from (type * p, int pitch) {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = *(p + y*pitch + x);
            image->updates += XYWH(origin.x, origin.y, size.x, size.y);
        }
        void copy_to (type * p, int pitch) const {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) *(p + y*pitch + x) = (*this)(x,y);
        }
    };
} 
