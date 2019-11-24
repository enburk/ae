#include "pch.h"
#include "../../_xpp/aux_array.h"
#include "../../_xpp/aux_string.h"
namespace test_aux
{
    TEST(TestAuxArray, Append)
    {
        array<str> a, b, c;
        a += "1";             ASSERT_EQ(a.size(), 1) << "1";
        b += a;               ASSERT_EQ(b.size(), 1) << "2"; ASSERT_EQ(a.size(), 1) << "2";
        b += b;               ASSERT_EQ(b.size(), 2) << "3"; ASSERT_EQ(a.size(), 1) << "3";
        b += std::move(a[0]); ASSERT_EQ(b.size(), 3) << "4"; ASSERT_EQ(a.size(), 1) << "4"; ASSERT_EQ(a[0], "");
        b += str("b");        ASSERT_EQ(b.size(), 4) << "5"; ASSERT_EQ(a.size(), 1) << "5";
        c += std::move(b);    ASSERT_EQ(c.size(), 4) << "6"; ASSERT_EQ(b.size(), 4) << "6"; ASSERT_EQ(b[0], ""); ASSERT_EQ(b[3], "");
        c += a;               ASSERT_EQ(c.size(), 5) << "7"; ASSERT_EQ(a.size(), 1) << "7";
    }

    TEST(TestAuxArray, Range)
    {
        array<int> a = {1, 2, 3};

        ASSERT_EQ(a.range().size(), 3);
        ASSERT_EQ(a.range(0).size(), 3);
        ASSERT_EQ(a.range(1).size(), 2);
        ASSERT_EQ(a.range(2).size(), 1);
        ASSERT_EQ(a.range(3).size(), 0);
        ASSERT_EQ(a.range(0, 0).size(), 0);
        ASSERT_EQ(a.range(0, 1).size(), 1);
        ASSERT_EQ(a.range(0, 2).size(), 2);
        ASSERT_EQ(a.range(0, 3).size(), 3);
        ASSERT_EQ(a.range(1, 1).size(), 0);
        ASSERT_EQ(a.range(1, 2).size(), 1);
        ASSERT_EQ(a.range(1, 3).size(), 2);
        ASSERT_EQ(a.range(2, 2).size(), 0);
        ASSERT_EQ(a.range(2, 3).size(), 1);
        ASSERT_EQ(a.range(3, 3).size(), 0);
    }

    TEST(TestAuxArray, RangeWithEnding)
    {
        array<char> a = {'a', 'b', 'c'};
        array<char> b = {'a', 'b', 'c', 'd'};

        ASSERT_EQ((a.range() + 'd').size(), 4);

        long l = 0; for (auto [c, n] : a.range() + 'd')
        {
            ASSERT_GE(n, 0);
            ASSERT_LE(n, 3);
            EXPECT_EQ(c, b[n]);
            EXPECT_EQ(n, l++);
        }

        ASSERT_EQ((a.range(0, 0) + 'd').size(), 1);

        for (auto [c, n] : a.range(0, 0) + 'd')
        {
            EXPECT_EQ(c, 'd');
            EXPECT_EQ(n,  0 );
        }
    }
}
