#include "pch.h"
#include "../../_xpp/pix_coord.h"
using namespace pix;
namespace test_pix
{
    TEST(TestPixCoord, XYXY)
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
}
