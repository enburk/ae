#include "pch.h"
#include "../../_xpp/pix_coord.h"
using namespace pix;
namespace test_pix
{
    TEST(TestPixCoord, XYWH)
    {
        EXPECT_EQ(XYWH(1,2,3,4).x, 1);
        EXPECT_EQ(XYWH(1,2,3,4).origin.y, 2);
        EXPECT_EQ(XYWH(1,2,3,4).width, 3);
        EXPECT_EQ(XYWH(1,2,3,4).size.y, 4);

        EXPECT_EQ(XYWH(1,2,3,4).local(), XYWH(0,0,3,4));
        EXPECT_EQ(XYWH(1,2,3,4)-XY(1,2), XYWH(0,0,3,4));
        EXPECT_EQ(XYWH(0,0,3,4)+XY(1,2), XYWH(1,2,3,4));
    }

    TEST(TestPixCoord, XYXY)
    {
        EXPECT_EQ(XYXY(1,2,3,4).left, 1);
        EXPECT_EQ(XYXY(1,2,3,4).origin.y, 2);
        EXPECT_EQ(XYXY(1,2,3,4).right_bottom.x, 3);
        EXPECT_EQ(XYXY(1,2,3,4).p2.y, 4);

        EXPECT_EQ(XYXY(1,2,3,4).local(), XYXY(0,0,2,2));
        EXPECT_EQ(XYXY(1,2,3,4)-XY(1,2), XYXY(0,0,2,2));
        EXPECT_EQ(XYXY(0,0,2,2)+XY(1,2), XYXY(1,2,3,4));
    }

    TEST(TestPixCoord, Conversion)
    {
        EXPECT_EQ(XYWH(XYXY()), XYWH());
        EXPECT_EQ(XYXY(XYWH()), XYXY());
        EXPECT_EQ(XYXY(XYWH(XYXY(1,2,3,4))), XYXY(1,2,3,4));
        EXPECT_EQ(XYWH(XYXY(XYWH(1,2,3,4))), XYWH(1,2,3,4));
        EXPECT_EQ(XYXY(1,2,3,4), XYWH(1,2,2,2));
        EXPECT_EQ(XYWH(1,2,3,4), XYXY(1,2,4,6));
    }

    TEST(TestPixCoord, Intersection)
    {
        EXPECT_EQ(XYWH() | XYXY(), XYWH());
        EXPECT_EQ(XYXY() | XYWH(), XYXY());
        EXPECT_EQ(XYWH() & XYXY(), XYWH());
        EXPECT_EQ(XYXY() & XYWH(), XYXY());
        EXPECT_EQ(XYXY(1,2,3,4) | XYWH(1,2,2,2), XYWH(1,2,2,2));
        EXPECT_EQ(XYWH(1,2,3,4) | XYXY(1,2,4,6), XYXY(1,2,4,6));
        EXPECT_EQ(XYXY(1,2,3,4) & XYWH(1,2,2,2), XYWH(1,2,2,2));
        EXPECT_EQ(XYWH(1,2,3,4) & XYXY(1,2,4,6), XYXY(1,2,4,6));

        EXPECT_EQ(XYXY(1,2,3,4) | XYXY(0,3,3,6), XYXY(0,2,3,6));
        EXPECT_EQ(XYXY(1,2,3,4) & XYXY(0,3,3,6), XYXY(1,3,3,4));
        EXPECT_EQ(XYWH(XYXY(1,2,3,4)) | XYWH(XYXY(0,3,3,6)), XYWH(XYXY(0,2,3,6)));
        EXPECT_EQ(XYWH(XYXY(1,2,3,4)) & XYWH(XYXY(0,3,3,6)), XYWH(XYXY(1,3,3,4)));
    }

    TEST(TestPixCoord, Include)
    {
        XYXY r (1,2,3,4);
        EXPECT_TRUE(r.excludes(XY(0,0))); EXPECT_TRUE(r.excludes(XY(1,0))); EXPECT_TRUE(r.excludes(XY(2,0))); EXPECT_TRUE(r.excludes(XY(3,0))); 
        EXPECT_TRUE(r.excludes(XY(0,1))); EXPECT_TRUE(r.excludes(XY(1,1))); EXPECT_TRUE(r.excludes(XY(2,1))); EXPECT_TRUE(r.excludes(XY(3,1))); 
        EXPECT_TRUE(r.excludes(XY(0,2))); EXPECT_TRUE(r.includes(XY(1,2))); EXPECT_TRUE(r.includes(XY(2,2))); EXPECT_TRUE(r.excludes(XY(3,2))); 
        EXPECT_TRUE(r.excludes(XY(0,3))); EXPECT_TRUE(r.includes(XY(1,3))); EXPECT_TRUE(r.includes(XY(2,3))); EXPECT_TRUE(r.excludes(XY(3,3))); 
        EXPECT_TRUE(r.excludes(XY(0,4))); EXPECT_TRUE(r.excludes(XY(1,4))); EXPECT_TRUE(r.excludes(XY(2,4))); EXPECT_TRUE(r.excludes(XY(3,4))); 
    }

    TEST(TestPixCoord, Exclude)
    {
        XYWH r (1,2,2,2);
        EXPECT_TRUE(r.excludes(XY(0,0))); EXPECT_TRUE(r.excludes(XY(1,0))); EXPECT_TRUE(r.excludes(XY(2,0))); EXPECT_TRUE(r.excludes(XY(3,0))); 
        EXPECT_TRUE(r.excludes(XY(0,1))); EXPECT_TRUE(r.excludes(XY(1,1))); EXPECT_TRUE(r.excludes(XY(2,1))); EXPECT_TRUE(r.excludes(XY(3,1))); 
        EXPECT_TRUE(r.excludes(XY(0,2))); EXPECT_TRUE(r.includes(XY(1,2))); EXPECT_TRUE(r.includes(XY(2,2))); EXPECT_TRUE(r.excludes(XY(3,2))); 
        EXPECT_TRUE(r.excludes(XY(0,3))); EXPECT_TRUE(r.includes(XY(1,3))); EXPECT_TRUE(r.includes(XY(2,3))); EXPECT_TRUE(r.excludes(XY(3,3))); 
        EXPECT_TRUE(r.excludes(XY(0,4))); EXPECT_TRUE(r.excludes(XY(1,4))); EXPECT_TRUE(r.excludes(XY(2,4))); EXPECT_TRUE(r.excludes(XY(3,4))); 
    }

    TEST(TestPixCoord, Rectifier)
    {
        XYWH good (0, 0, 1, 1); XYWH bad (0, 0, -1, -1);

        Rectifier r; EXPECT_EQ(r.size(), 0) << "line 0";
        r += XYWH(); EXPECT_EQ(r.size(), 0) << "line 1";
        r += bad;    EXPECT_EQ(r.size(), 0) << "line 2";
        r += good;   EXPECT_EQ(r.size(), 1) << "line 3";
        r += XYWH(); EXPECT_EQ(r.size(), 1) << "line 4";
        r += bad;    EXPECT_EQ(r.size(), 1) << "line 5";
        r += good;   EXPECT_EQ(r.size(), 1) << "line 6";
    }

    TEST(TestPixCoord, Rectifier1)
    {
        Rectifier r;
        r += XYWH(0,0, 1,2);
        r += XYWH(1,0, 1,2); ASSERT_EQ(r.size(), 1); EXPECT_EQ(r.rectangles[0], XYWH(0,0, 2,2));
        r += XYWH(9,0, 1,2); ASSERT_EQ(r.size(), 2);
    }
}
