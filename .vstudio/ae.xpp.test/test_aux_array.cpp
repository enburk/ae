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

    //TEST(TestAuxArray, RangeWithEnding)
    //{
    //}
}
