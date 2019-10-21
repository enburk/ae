#pragma once
#include "ide_test_fonts.h"
#include "ide_test_pix.h"
namespace ide::test
{
    namespace mouse
    {
        inline XY   pos;
        inline bool l_pressed = false;
        inline bool m_pressed = false;
        inline bool r_pressed = false;
        inline bool x_pressed = false;
        inline bool test = false;
        inline Image<XRGB> back;
        inline Image<XRGB> fore;

        namespace on
        {
            bool press(XY p, char button, bool down)
            {
                if (!test) return false;
                switch(button) {
                case 'L': l_pressed = down; break;
                case 'M': m_pressed = down; break;
                case 'R': r_pressed = down; break;
                case 'X': x_pressed = down; break;
                }
                return true;
            }
            bool move(XY p)
            {
                if (!test) return false;
                fore.fill(x_pressed ? XRGB::green : l_pressed ? XRGB::navy : XRGB::blue);
                back.copy_to  (sys::window::image.frame(pos-back.size/2)); pos = p;
                back.copy_from(sys::window::image.frame(pos-back.size/2));
                fore.copy_to  (sys::window::image.frame(pos-fore.size/2));
                return true;
            }
            bool clickclick(XY p, char button)
            {
                if (!test) return false;
                return true;
            }
        }
    }

    namespace window
    {
        namespace on
        {
            bool resize()
            {
                if (!test) return false;
                sys::window::image.fill(XRGB::red);
                if (0) pix  ::test().copy_to(sys::window::image.frame());
                if (1) fonts::test().copy_to(sys::window::image.frame());

                if (mouse::test) { int a = sys::window::image.size.x/200;
                    mouse::back.resize(XY(a,a));
                    mouse::fore.resize(XY(a,a));
                    mouse::pos = XY(-a,-a);
                }
                return true;
            }
            bool redraw(XYXY r)
            {
                if (!test) return false;
                return true;
            }
        }
    }
}
 