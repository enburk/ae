#include "sys.h"

void sys::window::on::resize()
{
}

void sys::window::on::redraw(XYXY r)
{
    sys::window::image.fill(XRGB::red);
}
 