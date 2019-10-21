#pragma once
#include "pix_coord.h"
namespace pix
{
    template<typename T> struct Frame;
    template<typename T> struct Image
    {
        typedef T type; std::vector<type> data; XY size; Rectifier updates;

	    explicit Image (               ) : data (                  ), size (0, 0) { updates = XYXY(*this); }
	    explicit Image (XY size        ) : data (size.x * size.y   ), size (size) { updates = XYXY(*this); }
	    explicit Image (XY size, type c) : data (size.x * size.y, c), size (size) { updates = XYXY(*this); }

        template<typename U> explicit Image (Frame<U> f) : Image (f.size)
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = type (f(x,y));
        }
        const
        type& operator () (int x, int y) const { return data[y*size.x + x]; }
        type& operator () (int x, int y)       { return data[y*size.x + x]; }

        explicit operator XYWH () const { return XYWH (0, 0, size.x, size.y); }
        explicit operator XYXY () const { return XYXY (0, 0, size.x, size.y); }

        Frame<type> frame (      ) { return Frame<type>(*this); }
        Frame<type> frame (XYWH r) { return frame().frame(r); }

	    void resize (XY Size) { data.resize (Size.x * Size.y); size = Size; updates = XYXY(*this); }

        void fill (const type & c) { for (auto & v : data) v = c; updates = XYXY(*this); }
    };

    template<typename T> struct Frame
    {
        typedef T type; Image<type> * image; XY offset, size;

        Frame (                   ) : image (nullptr) {}
        Frame (Image<type> & image) : image (&image), size(image.size) {}
        const
        type& operator () (int x, int y) const { return (*image)(offset.x+x, offset.y+y); }
        type& operator () (int x, int y)       { return (*image)(offset.x+x, offset.y+y); }

        bool  operator == (const Frame & f) const { return image == f.image && offset == f.offset && size == f.size; }
        bool  operator != (const Frame & f) const { return image != f.image || offset != f.offset || size != f.size; }

        explicit operator XYWH () const { return XYWH (offset.x, offset.y, size.x, size.y); }
        explicit operator XYXY () const { return XYWH (*this); }

        Frame frame (XYWH r) const {
            r = XYWH(*this) & (r + offset);
            Frame f;
            f.image  = image;
            f.offset = XY(r.x, r.y);
            f.size   = XY(r.w, r.h);
            return f;
        }

        void  fill (const type & c) {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = c;
            image->updates += XYWH(*this);
        }
        void  blend (const type & c, uint8_t alpha = 255) {
            if (alpha == 0) return;
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y).blend (c, alpha);
            image->updates += XYWH(*this);
        }

        void copy_from (Frame f) {
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y) = f(x,y);
            image->updates += XYWH(offset.x, offset.y, w, h);
        }
        void copy_to (Frame f) const { f.copy_from (*this); }

        void blend_from (Frame f, uint8_t alpha = 255) {
            if (alpha == 0) return;
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y).blend(f(x,y), alpha);
            image->updates += XYWH(offset.x, offset.y, w, h);
        }
        void blend_to (Frame f, uint8_t alpha = 255) const { f.blend_from (*this, alpha); }

        void copy_from (type * p, int pitch) {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = *(p + y*pitch + x);
            image->updates += XYWH(*this);
        }
        void copy_to (type * p, int pitch) const {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) *(p + y*pitch + x) = (*this)(x,y);
        }
    };
} 
