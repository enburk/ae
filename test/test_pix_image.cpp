#include "pch.h"
#include "../../_xpp/pix_image.h"
using namespace pix;
namespace test_pix
{
    TEST(TestPixImage, Image)
    {
        image<int> i (XY(3,3), 9);

        EXPECT_NO_THROW({ i.crop(XYWH(-2,-2, 3, 3)).fill(1); });
        EXPECT_NO_THROW({ i.crop(XYWH( 2,-2, 3, 3)).fill(2); });
        EXPECT_NO_THROW({ i.crop(XYWH( 2, 2, 3, 3)).fill(3); });
        EXPECT_NO_THROW({ i.crop(XYWH(-2, 2, 3, 3)).fill(4); });

        EXPECT_EQ(i(0,0), 1); EXPECT_EQ(i(1,0), 9); EXPECT_EQ(i(2,0), 2);
        EXPECT_EQ(i(0,1), 9); EXPECT_EQ(i(1,1), 9); EXPECT_EQ(i(2,1), 9);
        EXPECT_EQ(i(0,2), 4); EXPECT_EQ(i(1,2), 9); EXPECT_EQ(i(2,2), 3);
    }
}
