#include "pch.h"
#include "../../_xpp/aux_string.h"
namespace test_aux
{
    TEST(TestAuxString, Find)
    {
        ASSERT_ANY_THROW(str("").find(""));
        ASSERT_ANY_THROW(str("").find(str::one_of("")));
        ASSERT_ANY_THROW(str("").find(str::one_not_of{ "" }));

        EXPECT_EQ(str("").find("b").size, 0);
        EXPECT_EQ(str("").find(str::one_of("b")).size, 0);
        EXPECT_EQ(str("").find(str::one_not_of("b")).size, 0);

        EXPECT_EQ(str("b").find("b").size, 1);
        EXPECT_EQ(str("b").find(str::one_of{ "b" }).size, 1);
        EXPECT_EQ(str("b").find(str::one_not_of{ "b" }).size, 0);

        EXPECT_EQ(str("abccba").find("b").pos, 1);
        EXPECT_EQ(str("abccba").find("b").size, 1);
        EXPECT_EQ(str("abccba").find("d").size, 0);
        EXPECT_EQ(str("abccba").find("cc").pos, 2);
        EXPECT_EQ(str("abccba").find("cc").size, 2);
    }

    TEST(TestAuxString, Split)
    {
        ASSERT_ANY_THROW(str("").split_by(""));
        ASSERT_ANY_THROW(str("|").split_by(""));
        ASSERT_EQ(str("").split_by("|").size(), 1);
        EXPECT_EQ(str("").split_by("|")[0], "");
        ASSERT_EQ(str("a").split_by("|").size(), 1);
        EXPECT_EQ(str("a").split_by("|")[0], "a");
        ASSERT_EQ(str("|").split_by("|").size(), 2);
        EXPECT_EQ(str("|").split_by("|")[0], "");
        EXPECT_EQ(str("|").split_by("|")[1], "");
        ASSERT_EQ(str("a|").split_by("|").size(), 2);
        EXPECT_EQ(str("a|").split_by("|")[0], "a");
        EXPECT_EQ(str("a|").split_by("|")[1], "");
        ASSERT_EQ(str("|b").split_by("|").size(), 2);
        EXPECT_EQ(str("|b").split_by("|")[0], "");
        EXPECT_EQ(str("|b").split_by("|")[1], "b");
        ASSERT_EQ(str("a|b").split_by("|").size(), 2);
        EXPECT_EQ(str("a|b").split_by("|")[0], "a");
        EXPECT_EQ(str("a|b").split_by("|")[1], "b");
    }
}
