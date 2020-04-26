#pragma once
#include "pix_color.h"
#include "pix_image.h"
namespace pix::sampling
{
    template<class color> inline
    color linear (frame<color> frame, double x, double y)
    {
        int sx = frame.size.x; if (sx <= 0) return color();
        int sy = frame.size.y; if (sy <= 0) return color();

        int x0 = (int) x;  int x1 = x0 + 1;  double dx = x - x0;
        int y0 = (int) y;  int y1 = y0 + 1;  double dy = y - y0;

        x0 = aux::clamp(x0, 0, sx-1); y0 = aux::clamp(y0, 0, sy-1);
        x1 = aux::clamp(x1, 0, sx-1); y1 = aux::clamp(y1, 0, sy-1);

        color co00 = frame (x0, y0);
        color co01 = frame (x0, y1);
        color co10 = frame (x1, y0);
        color co11 = frame (x1, y1);

        co00.blend(co10, (int)(255*dx));
        co10.blend(co11, (int)(255*dx));
        co00.blend(co10, (int)(255*dy));

        return co00;
    }

    template<class color> inline
    color cubic (frame<color> & frame, double x, double y)
    {
        color co = pix::black;

        int xx = (int) x; double dx = x - xx;
        int yy = (int) y; double dy = y - yy;

        double dx2 = dx*dx; double dx3 = dx*dx2;
        double dy2 = dy*dy; double dy3 = dy*dy2;

        auto source = [&frame](int x, int y)
        {
            x = aux::clamp(x, 0, frame.size.x-1);
            y = aux::clamp(y, 0, frame.size.y-1);
            
            return frame(x,y);
        };

        for (int channel=0; channel<color::total_channels; channel++)
        {
            double c, cc [4];

            for (int ypi=-1;  ypi<=2;  ypi++)
            {
                int c0 = source(xx-1, yy+ypi).channels[channel];
                int c1 = source(xx-0, yy+ypi).channels[channel];
                int c2 = source(xx+1, yy+ypi).channels[channel];
                int c3 = source(xx+2, yy+ypi).channels[channel];

                double d0 = double (c0 - c1);
                double d2 = double (c2 - c1);
                double d3 = double (c3 - c1);

                double a0 = double (c1);
                double a1 = -d0/3 + d2   - d3/6;
                double a2 =  d0/2 + d2/2;
                double a3 = -d0/6 - d2/2 + d3/6;

                cc[ypi+1] = a0 + a1*dx + a2*dx2 + a3*dx3;
            }

            double d0 = cc[0] - cc[1];
            double d2 = cc[2] - cc[1];
            double d3 = cc[3] - cc[1];

            double a0 = cc[1];
            double a1 = -d0/3 + d2   - d3/6;
            double a2 =  d0/2 + d2/2;
            double a3 = -d0/6 - d2/2 + d3/6;

            c = a0 + a1*dy + a2*dy2 + a3*dy3;

            co.channels[channel] = aux::clamp<uint8_t>(c);
        }

        return co;
    }
}

namespace pix
{
    template<class color> inline
    image<color> resized (frame<color> src, XY size)
    {
        image<color> dst(size);

        auto sample = [size, &src](double x, double y) {
            return size.x * size.y < src.size.x * src.size.y ?
                sampling::linear(src, x, y):
                sampling::cubic (src, x, y);
        };

        vector<2> ax ({1.0f / size.x * src.size.x, 0.0f});
        vector<2> ay ({0.0f, 1.0f / size.y * src.size.y});

        for( int y=0; y<dst.size.y; y++ )
        for( int x=0; x<dst.size.x; x++ )
        {
            vector<2> p = x * ax + y * ay;

            if (p.x < 0 or src.size.x <= p.x
            or  p.y < 0 or src.size.y <= p.y) continue;

            dst(x,y) = sample(p.x, p.y);
        }

        return dst;
    }

    template<class color> inline
    void resize (image<color> & img, XY size)
    {
        img = resized(img.crop(), size);
    }
}

