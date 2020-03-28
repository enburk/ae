#include "pch.h"
#include "../library/cpp/aux_array.h"
namespace aux
{
    TEST(TestPixMath, Vector)
    {
        using vector2d = vector<2, double>;
        using vector2u = vector<2, uint8_t>;
        EXPECT_EQ(vector<2>().x, 0.0f);
        EXPECT_EQ(vector<2>().y, 0.0f);
        EXPECT_EQ(vector2d().x, 0.0);
        EXPECT_EQ(vector2d().y, 0.0);
        EXPECT_EQ(vector2u().x, 0);
        EXPECT_EQ(vector2u().y, 0);

        //// vector<3> va({ 1.0, 2.0 }); // should not compile
        /**/ vector<3> vb({ 1.0, 2.0, 3.0 }); // OK
        //// vector<3> vc({ 1.0, 2.0, 3.0, 4.0 }); // should not compile

        double epsilon = 0.000001;
        using vector1f = vector<1, float>;
        using vector1d = vector<1, double>;
        using vector1u = vector<1, uint8_t>;
        using vector1i = vector<1, int>;

        EXPECT_LT(abs(clamp<double>((unsigned char)(255)) - 255.0), epsilon);
        EXPECT_GT(abs(clamp<double>((unsigned char)(255)) - 255.1), epsilon);
        EXPECT_GT(abs(clamp<double>((unsigned char)(255)) - 254.9), epsilon);

        EXPECT_LT(abs(clamp<double>((signed char)(-128)) + 128.0), epsilon);
        EXPECT_GT(abs(clamp<double>((signed char)(-128)) + 128.1), epsilon);
        EXPECT_GT(abs(clamp<double>((signed char)(-128)) + 128.9), epsilon);

        EXPECT_LT(abs(vector1d({ 1 }).x - 1.0f), epsilon);
        EXPECT_LT(abs(vector1f({ 1 }).x - 1.0f), epsilon);
        EXPECT_LT(abs(vector1d({ 1.0 }).x - 1.0f), epsilon);
        EXPECT_LT(abs(vector1f({ 1.0 }).x - 1.0f), epsilon);

        EXPECT_EQ(vector1i({ 256 }).x, 256);
        EXPECT_EQ(vector1u({ 256 }).x, 255);
        EXPECT_EQ(vector1i({ 256.5 }).x, 256);
        EXPECT_EQ(vector1u({ 256.5 }).x, 255);
        EXPECT_EQ(vector1u({ -256 }).x, 0);
        EXPECT_EQ(vector1u({ -256.5 }).x, 0);

        EXPECT_EQ(vector<3>({ 1.0f, 2.0f, 3.0f }).x, 1.0f);
        EXPECT_EQ(vector<3>({ 1.0f, 2.0f, 3.0f }).y, 2.0f);
        EXPECT_EQ(vector<3>({ 1.0f, 2.0f, 3.0f }).z, 3.0f);

        EXPECT_EQ(vector2u({ 0, 0 }) + vector2u({ 0, 0 }), vector2u({ 0, 0 }));
        EXPECT_EQ(vector2u({ 1, 0 }) + vector2u({ 1, 0 }), vector2u({ 2, 0 }));
        EXPECT_EQ(vector2u({ 1, 2 }) + vector2u({ 1, 2 }), vector2u({ 2, 4 }));

        EXPECT_LT(distance(vector1d({ 255 }), vector1d({ 255.0 })), epsilon);
        EXPECT_LT(distance(vector1d({ char(127) }), vector1d({ 127.0 })), epsilon);
        EXPECT_LT(distance(vector1d({ (unsigned char)(255) }), vector1d({ 255.0 })), epsilon);
        EXPECT_GT(distance(vector1d({ (unsigned char)(255) }), vector1d({ 255.1 })), epsilon);
        EXPECT_GT(distance(vector1d({ (unsigned char)(255) }), vector1d({ 254.9 })), epsilon);

        EXPECT_LT(distance(vector<2>({ 0, 0 }) + vector<2>({ 0, 0 }), vector<2>({ 0, 0 })), epsilon);
        EXPECT_LT(distance(vector<2>({ 1, 0 }) + vector<2>({ 1, 0 }), vector<2>({ 2, 0 })), epsilon);
        EXPECT_LT(distance(vector<2>({ 1, 2 }) + vector<2>({ 1, 2 }), vector<2>({ 2, 4 })), epsilon);

        EXPECT_LT(distance(vector<2>({ 0, 0 }) - vector<2>({ 0, 0 }), vector<2>({ 0, 0 })), epsilon);
        EXPECT_LT(distance(vector<2>({ 1, 0 }) - vector<2>({ 1, 0 }), vector<2>({ 0, 0 })), epsilon);
        EXPECT_LT(distance(vector<2>({ 1, 2 }) - vector<2>({ 1, 2 }), vector<2>({ 0, 0 })), epsilon);

        EXPECT_LT(distance(vector<2>({ 0.0, 0.0 }) * 2, vector<2>({ 0.0, 0.0 })), epsilon);
        EXPECT_LT(distance(vector<2>({ 1.0, 0.0 }) * 2, vector<2>({ 2.0, 0.0 })), epsilon);
        EXPECT_LT(distance(vector<2>({ 1.0, 2.0 }) * 2, vector<2>({ 2.0, 4.0 })), epsilon);

        EXPECT_LT(distance(vector<2>({ 0.0, 0.0 }) / 2, vector<2>({ 0.0, 0.0 })), epsilon);
        EXPECT_LT(distance(vector<2>({ 1.0, 0.0 }) / 2, vector<2>({ 0.5, 0.0 })), epsilon);
        EXPECT_LT(distance(vector<2>({ 1.0, 2.0 }) / 2, vector<2>({ 0.5, 1.0 })), epsilon);

        EXPECT_LT(distance(2 * vector<2>({ 0.0, 0.0 }), vector<2>({ 0.0, 0.0 })), epsilon);
        EXPECT_LT(distance(2 * vector<2>({ 1.0, 0.0 }), vector<2>({ 2.0, 0.0 })), epsilon);
        EXPECT_LT(distance(2 * vector<2>({ 1.0, 2.0 }), vector<2>({ 2.0, 4.0 })), epsilon);

        EXPECT_LT(abs(vector<2>({ 0.0, 0.0 }) * vector<2>({ 0.0, 0.0 }) - 0.0), epsilon);
        EXPECT_LT(abs(vector<2>({ 1.0, 0.0 }) * vector<2>({ 0.0, 1.0 }) - 0.0), epsilon);
        EXPECT_LT(abs(vector<2>({ 1.0, 0.0 }) * vector<2>({ 1.0, 0.0 }) - 1.0), epsilon);
        EXPECT_LT(abs(vector<2>({ 1.0, 2.0 }) * vector<2>({ 1.0, 2.0 }) - 5.0), epsilon);
    }
}
