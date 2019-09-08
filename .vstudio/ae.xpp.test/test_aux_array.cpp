#include "pch.h"
#include "../../_xpp/aux_array.h"
namespace test_aux
{
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

#ifndef _DEBUG // "cannot seek vector iterator after end"
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
#endif
}
