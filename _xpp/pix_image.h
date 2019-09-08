#pragma once
#include "aux_array.h"
#include "pix_coord.h"
namespace pix
{
    template<typename T> struct FRAME;
    template<typename T> struct IMAGE
    {
        typedef T type; std::vector<type> data; XY size; array<XYXY> update;

	    explicit IMAGE (               ) : data (                  ), size (0, 0) { update += XYXY(*this); }
	    explicit IMAGE (XY size        ) : data (size.x * size.y   ), size (size) { update += XYXY(*this); }
	    explicit IMAGE (XY size, type c) : data (size.x * size.y, c), size (size) { update += XYXY(*this); }

        template<typename U> explicit IMAGE (FRAME<U> f) : IMAGE (f.size())
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) at(x,y) = type (f.at(x,y));
        }
        const
        type& at (int x, int y) const { return data[y*size.x + x]; }
        type& at (int x, int y)       { return data[y*size.x + x]; }
        const
        type& operator () (int x, int y) const { return at(x,y); }
        type& operator () (int x, int y)       { return at(x,y); }

        explicit operator XYWH () const { return XYWH (0, 0, size.x, size.y); }
        explicit operator XYXY () const { return XYXY (0, 0, size.x, size.y); }

	    void resize (XY Size) { data.resize (Size.x * Size.y); size = Size; update.clear(); update += XYXY(*this); }

        void fill (const type & c) { for (auto & v : data) v = c; update.clear(); update += XYXY(*this); }

        FRAME<type> frame (      ) { return FRAME<type>(this); }
        FRAME<type> frame (XY p  ) { return FRAME<type>(this, p); }
        FRAME<type> frame (XYWH r) { return FRAME<type>(this, r); }

        void copy_to   (FRAME<type> f) { frame().copy_to  (f); }
        void copy_from (FRAME<type> f) { frame().copy_from(f); }
    };

    template<typename T> struct FRAME
    {
        typedef T type; IMAGE<type> * image; XY origin, size;

        FRAME (IMAGE<type> * image=nullptr) : image (image) { if (image) size = image->size; }
        FRAME (IMAGE<type> * image, XY   p) : image (image) { if (image) size = image->size; *this = frame(p); }
        FRAME (IMAGE<type> * image, XYWH r) : image (image) { if (image) size = image->size; *this = frame(r); }
 
        FRAME (IMAGE<type> & image        ) : FRAME(&image) {}
        FRAME (IMAGE<type> & image, XY   p) : FRAME(&image, p) {}
        FRAME (IMAGE<type> & image, XYWH r) : FRAME(&image, r) {}
 
        const
        type& at (int x, int y) const { return image->at(origin.x+x, origin.y+y); }
        type& at (int x, int y)       { return image->at(origin.x+x, origin.y+y); }
        const
        type& operator () (int x, int y) const { return at(x,y); }
        type& operator () (int x, int y)       { return at(x,y); }

        bool  operator == (const FRAME & e) const { return image == e.image && origin == e.origin && size == e.size; }
        bool  operator != (const FRAME & e) const { return image != e.image || origin != e.origin || size != e.size; }

        explicit operator XYWH () const { return XYWH (origin.x, origin.y, size.x, size.y); }
        explicit operator XYXY () const { return XYXY (origin.x, origin.y, size.x, size.y); }

        FRAME frame (XY p) const {
            int x = min(size.x, max(origin.x+p.x, 0)); int w = max(size.x-x, 0);
            int y = min(size.y, max(origin.y+p.y, 0)); int h = max(size.y-y, 0);
            FRAME f; f.image = image; f.origin = XY(x,y); f.size = XY(w,h); return f;
        }
        FRAME frame (XYWH r) const {
            int x = min(size.x, max(origin.x+r.x, 0)); int w = min(size.x-x, max(r.w, 0));
            int y = min(size.y, max(origin.y+r.y, 0)); int h = min(size.y-y, max(r.h, 0));
            FRAME f; f.image = image; f.origin = XY(x,y); f.size = XY(w,h); return f;
        }

        void  fill (const type & c) {
            if  (!image) return;
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) at(x,y) = c;
        }
        void  blend (const type & c, uint8_t alpha = 255) {
            if  (!image) return;
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) at(x,y).blend (c, alpha);
        }

        void copy_from (FRAME f) {
            if  (!image) return;
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) at(x,y) = f(x,y);
            image->update += XYWH(origin.x, origin.y, w, h);
        }
        void copy_to (FRAME f) const { f.copy_from (*this); }

        void blend_from (FRAME f, uint8_t alpha = 255) {
            if  (!image) return;
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) at(x,y).blend(f(x,y), alpha);
            image->update += XYWH(origin.x, origin.y, w, h);
        }
        void blend_to (FRAME f, uint8_t alpha = 255) const { f.blend_from (*this, alpha); }

        void copy_from (type * p, int pitch) {
            if  (!image) return;
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) at(x,y) = *(p + y*pitch + x);
            image->update += XYWH(origin.x, origin.y, size.x, size.y);
        }
        void copy_to (type * p, int pitch) const {
            if  (!image) return;
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) *(p + y*pitch + x) = at(x,y);
        }
    };
} 
