#pragma once
#include "sys.h"
namespace ide::test::pix
{
    static Image<XRGB> test ()
    {
        Image<XRGB> image (sys::window::image.size, XRGB::gray);
        int dx = image.size.x/100;
        int dy = dx*13/8;

        Image<XRGB> r (XY(dx,dy), XRGB::red);
        Image<XRGB> g (XY(dx,dy), XRGB::green);
        Image<XRGB> b (XY(dx,dy), XRGB::blue);

        image.frame(XY(dx*1, dy+1)).copy_from(r);
        image.frame(XY(dx*2, dy+2)).copy_from(g);
        image.frame(XY(dx*3, dy+3)).copy_from(b);

        return image;
    }
}