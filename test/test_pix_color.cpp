#include "pch.h"
#include "../../_xpp/pix_color.h"
using namespace pix;
namespace test_pix
{
    TEST(TestPixColor, ARGB)
    {
        EXPECT_EQ(pix::red.a, 255); EXPECT_EQ(pix::lime.a, 255); EXPECT_EQ(pix::blue.a, 255);
        EXPECT_EQ(pix::red.r, 255); EXPECT_EQ(pix::lime.r,   0); EXPECT_EQ(pix::blue.r,   0);
        EXPECT_EQ(pix::red.g,   0); EXPECT_EQ(pix::lime.g, 255); EXPECT_EQ(pix::blue.g,   0);
        EXPECT_EQ(pix::red.b,   0); EXPECT_EQ(pix::lime.b,   0); EXPECT_EQ(pix::blue.b, 255);
    }

    TEST(TestPixColor, Blend)
    {
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime,   0).a, 255);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime,   0).r, 255);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime,   0).g,   0);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime,   0).b,   0);
        // 
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime,  64).a, 255);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime,  64).r, 192);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime,  64).g,  62);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime,  64).b,   0);
        // 
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 128).a, 255);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 128).r, 128);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 128).g, 126);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 128).b,   0);
        // 
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 192).a, 255);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 192).r,  64);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 192).g, 190);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 192).b,   0);
        // 
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 255).a, 255);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 255).r,   0);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 255).g, 255);
        // EXPECT_EQ(RGBA(pix::red).blend(pix::lime, 255).b,   0);
    }

    TEST(TestPixColor, BlendPlus)
    {
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF),   0).a,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF),   0).r,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF),   0).g, 255);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF),   0).b,   0);
        // 
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF),  64).a,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF),  64).r,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF),  64).g, 223);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF),  64).b,  31);
        // 
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 128).a,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 128).r,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 128).g, 191);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 128).b,  63);
        // 
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 192).a,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 192).r,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 192).g, 159);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 192).b,  95);
        // 
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 255).a,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 255).r,   0);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 255).g, 127);
        // EXPECT_EQ(ARGB(0x0000FF00).blend(ARGB(0x800000FF), 255).b, 127);
    }
}