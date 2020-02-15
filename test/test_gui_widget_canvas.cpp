#include "pch.h"
#include "../../_xpp/gui_widget_canvas.h"
using namespace pix;
using namespace gui;
namespace test_gui
{
    TEST(TestGuiCanvas, Update)
    {
        struct A : widget<A> { gui::canvas canvas;
        struct B : widget<A> { gui::canvas canvas;
        struct C : widget<A> { gui::canvas canvas;
        } c; } b; } a;

        a.alpha = 0;                        ASSERT_EQ(a.updates.size(), 0) << "a0"; 
        a.b.alpha = 0;                      ASSERT_EQ(a.updates.size(), 0) << "a1"; 
        a.b.c.alpha = 0;                    ASSERT_EQ(a.updates.size(), 0) << "a2"; 

        a.coord = XYWH(1,1,3,3);            ASSERT_EQ(a.updates.size(), 0) << "b0"; 
        a.b.coord = XYWH(1,1,3,3);          ASSERT_EQ(a.updates.size(), 0) << "b1"; 
        a.b.c.coord = XYWH(1,1,3,3);        ASSERT_EQ(a.updates.size(), 0) << "b2"; 

        a.canvas.coord = XYWH(0,0,3,3);     ASSERT_EQ(a.updates.size(), 0) << "b00"; 
        a.b.canvas.coord = XYWH(0,0,3,3);   ASSERT_EQ(a.updates.size(), 0) << "b11"; 
        a.b.c.canvas.coord = XYWH(0,0,3,3); ASSERT_EQ(a.updates.size(), 0) << "b22"; 

        image<RGBA> i (XY(3,3), black); a.render(i, XY(0,0));
        EXPECT_EQ(i(0,0), black); EXPECT_EQ(i(1,0), black); EXPECT_EQ(i(2,0), black);
        EXPECT_EQ(i(0,1), black); EXPECT_EQ(i(1,1), black); EXPECT_EQ(i(2,1), black);
        EXPECT_EQ(i(0,2), black); EXPECT_EQ(i(1,2), black); EXPECT_EQ(i(2,2), black);

        a.canvas.color = red;               ASSERT_EQ(a.updates.size(), 0) << "c0"; 
        a.b.canvas.color = green;           ASSERT_EQ(a.updates.size(), 0) << "c1"; 
        a.b.c.canvas.color = blue;          ASSERT_EQ(a.updates.size(), 0) << "c2"; 

        a.alpha = 255;                      ASSERT_EQ(a.updates.size(), 0) << "d0"; // parent == nullptr
        a.b.alpha = 255;                    ASSERT_EQ(a.updates.size(), 1) << "d1"; 
        a.b.c.alpha = 255;                  ASSERT_EQ(a.updates.size(), 1) << "d2"; 

        a.render(i, XY(0,0));
        EXPECT_EQ(i(0,0), red  ); EXPECT_EQ(i(1,0), red  ); EXPECT_EQ(i(2,0), red  );
        EXPECT_EQ(i(0,1), red  ); EXPECT_EQ(i(1,1), green); EXPECT_EQ(i(2,1), green);
        EXPECT_EQ(i(0,2), red  ); EXPECT_EQ(i(1,2), green); EXPECT_EQ(i(2,2), blue );

        i.updates.clear();
        a.updates.clear();
        a.b.c.coord = XYWH(-2,-2, 3,3);
        ASSERT_EQ(a.updates.size(), 2) << "e0"; 
        ASSERT_EQ(a.updates.rectangles[0], XYWH(2,2,1,1)) << "e1"; // was
        ASSERT_EQ(a.updates.rectangles[1], XYWH(1,1,1,1)) << "e2"; // now

        for (auto r : a.updates) a.render(i.crop(r), r.origin);
        EXPECT_EQ(i(0,0), red  ); EXPECT_EQ(i(1,0), red  ); EXPECT_EQ(i(2,0), red  );
        EXPECT_EQ(i(0,1), red  ); EXPECT_EQ(i(1,1), blue ); EXPECT_EQ(i(2,1), green);
        EXPECT_EQ(i(0,2), red  ); EXPECT_EQ(i(1,2), green); EXPECT_EQ(i(2,2), green);
        ASSERT_EQ(i.updates.rectangles, a.updates.rectangles) << "ei"; 

        i.updates.clear();
        a.updates.clear();
        a.b.alpha = 0;
        ASSERT_EQ(a.updates.size(), 1) << "f0"; 
        ASSERT_EQ(a.updates.rectangles[0], XYWH(1,1,2,2)) << "f2";

        for (auto r : a.updates) a.render(i.crop(r), r.origin);
        EXPECT_EQ(i(0,0), red  ); EXPECT_EQ(i(1,0), red  ); EXPECT_EQ(i(2,0), red  );
        EXPECT_EQ(i(0,1), red  ); EXPECT_EQ(i(1,1), red  ); EXPECT_EQ(i(2,1), red  );
        EXPECT_EQ(i(0,2), red  ); EXPECT_EQ(i(1,2), red  ); EXPECT_EQ(i(2,2), red  );
        ASSERT_EQ(i.updates.rectangles, a.updates.rectangles) << "fi"; 

        i.updates.clear();
        a.updates.clear();
        a.b.alpha = 255;
        a.b.c.alpha = 0;
        ASSERT_EQ(a.updates.size(), 1) << "f0"; 
        ASSERT_EQ(a.updates.rectangles[0], XYWH(1,1,2,2)) << "f2";

        for (auto r : a.updates) a.render(i.crop(r), r.origin);
        EXPECT_EQ(i(0,0), red  ); EXPECT_EQ(i(1,0), red  ); EXPECT_EQ(i(2,0), red  );
        EXPECT_EQ(i(0,1), red  ); EXPECT_EQ(i(1,1), green); EXPECT_EQ(i(2,1), green);
        EXPECT_EQ(i(0,2), red  ); EXPECT_EQ(i(1,2), green); EXPECT_EQ(i(2,2), green);
        ASSERT_EQ(i.updates.rectangles, a.updates.rectangles) << "fi"; 
    }
}