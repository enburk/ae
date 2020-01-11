#include "pch.h"
#include "../../libraries/cpp/aux_string.hpp"
namespace aux
{
    TEST(TestAuxString, Substr)
    {
        ASSERT_EQ(str("").from(0).size(0), "");
        ASSERT_EQ(str("").from(0).size(1), "");
        ASSERT_EQ(str("").from(1).size(1), "");

        EXPECT_EQ(str("abc").from(1).size(0), "");
        EXPECT_EQ(str("abc").from(1).size(1), "b");
        EXPECT_EQ(str("abc").from(1).size(2), "bc");
        EXPECT_EQ(str("abc").from(1).size(3), "bc");

        EXPECT_EQ(str("abc").from(2), "c");
        EXPECT_EQ(str("abc").upto(2), "ab");

        EXPECT_EQ(str("abccba").substr(str::start_from(1)), "bccba");
        EXPECT_EQ(str("abccba").substr(str::start_from(2)), "ccba");
        EXPECT_EQ(str("abccba").substr(str::start_from(5)), "a");
        EXPECT_EQ(str("abccba").substr(str::start_from_end()), "abccba");
        EXPECT_EQ(str("abccba").substr(str::start_from_end(1)), "abccb");
        EXPECT_EQ(str("abccba").substr(str::start_from_end(2)), "abcc");
        EXPECT_EQ(str("abccba").substr(str::start_from_end(5)), "a");
    }

    TEST(TestAuxString, Find)
    {
        ASSERT_ANY_THROW(str("").find(""));
        ASSERT_ANY_THROW(str("").find(str::one_of("")));
        ASSERT_ANY_THROW(str("").find(str::one_not_of{ "" }));

        ASSERT_ANY_THROW(str("abc").find(""));
        ASSERT_ANY_THROW(str("abc").find(str::one_of("")));
        ASSERT_ANY_THROW(str("abc").find(str::one_not_of{ "" }));

        EXPECT_EQ(str("").find("b").length, 0);
        EXPECT_EQ(str("").find(str::one_of("b")).length, 0);
        EXPECT_EQ(str("").find(str::one_not_of("b")).length, 0);

        EXPECT_EQ(str("b").find("b").length, 1);
        EXPECT_EQ(str("b").find(str::one_of{ "b" }).length, 1);
        EXPECT_EQ(str("b").find(str::one_not_of{ "b" }).length, 0);

        EXPECT_EQ(str("abccba").find("b").offset, 1);
        EXPECT_EQ(str("abccba").find("b").length, 1);
        EXPECT_EQ(str("abccba").find("d").length, 0);
        EXPECT_EQ(str("abccba").find("cc").offset, 2);
        EXPECT_EQ(str("abccba").find("cc").length, 2);

        EXPECT_EQ(str("abccba").find("b", str::start_from(1)).offset, 1);
        EXPECT_EQ(str("abccba").find("b", str::start_from(2)).offset, 4);
        EXPECT_EQ(str("abccba").find("b", str::start_from(5)).length, 0);
        EXPECT_EQ(str("abccba").find("b", str::start_from_end()).offset, 4);
        EXPECT_EQ(str("abccba").find("b", str::start_from_end(1)).offset, 4);
        EXPECT_EQ(str("abccba").find("b", str::start_from_end(2)).offset, 1);
        EXPECT_EQ(str("abccba").find("b", str::start_from_end(5)).length, 0);
    }

    TEST(TestAuxString, Split)
    {
        ASSERT_ANY_THROW(str("").split_by(""));
        ASSERT_ANY_THROW(str("|").split_by(""));
        ASSERT_EQ(str("").split_by("|").size(), 1);
        ASSERT_EQ(str("a").split_by("|").size(), 1);
        ASSERT_EQ(str("|").split_by("|").size(), 2);
        ASSERT_EQ(str("a|").split_by("|").size(), 2);
        ASSERT_EQ(str("|b").split_by("|").size(), 2);
        ASSERT_EQ(str("a|b").split_by("|").size(), 2);

        EXPECT_EQ(str("").split_by("|")[0], "");
        EXPECT_EQ(str("a").split_by("|")[0], "a");
        EXPECT_EQ(str("|").split_by("|")[0], "");
        EXPECT_EQ(str("|").split_by("|")[1], "");
        EXPECT_EQ(str("a|").split_by("|")[0], "a");
        EXPECT_EQ(str("a|").split_by("|")[1], "");
        EXPECT_EQ(str("|b").split_by("|")[0], "");
        EXPECT_EQ(str("|b").split_by("|")[1], "b");
        EXPECT_EQ(str("a|b").split_by("|")[0], "a");
        EXPECT_EQ(str("a|b").split_by("|")[1], "b");

        str  a,b;
        str("a|b").split_by("|", a, b, str::delimiter::to_the_left);  EXPECT_EQ(a, "a|") << "split 1";
        str("a|b").split_by("|", a, b, str::delimiter::to_the_left);  EXPECT_EQ(b, "b" ) << "split 2";
        str("a|b").split_by("|", a, b, str::delimiter::to_the_right); EXPECT_EQ(a, "a" ) << "split 3";
        str("a|b").split_by("|", a, b, str::delimiter::to_the_right); EXPECT_EQ(b, "|b") << "split 4";
    }

    TEST(TestAuxString, Is)
    {
        EXPECT_TRUE(str("abc").starts_with(""));
        EXPECT_TRUE(str("abc").starts_with("a"));
        EXPECT_TRUE(str("abc").starts_with("ab"));
        EXPECT_TRUE(str("abc").starts_with("abc"));
        EXPECT_FALSE(str("abc").starts_with("abcd"));
        EXPECT_FALSE(str("abc").starts_with("abd"));
        EXPECT_FALSE(str("abc").starts_with("ad"));
        EXPECT_FALSE(str("abc").starts_with("d"));

        EXPECT_TRUE(str("abc").ends_with(""));
        EXPECT_TRUE(str("abc").ends_with("c"));
        EXPECT_TRUE(str("abc").ends_with("bc"));
        EXPECT_TRUE(str("abc").ends_with("abc"));
        EXPECT_FALSE(str("abc").ends_with("aabc"));
        EXPECT_FALSE(str("abc").ends_with("aab"));
        EXPECT_FALSE(str("abc").ends_with("aa"));
        EXPECT_FALSE(str("abc").ends_with("a"));

        EXPECT_TRUE(str("abccba").contains("cc"));
        EXPECT_TRUE(str("abccba").contains(str::one_of("acc")));
        EXPECT_TRUE(str("abccba").contains(str::one_not_of("acc")));

        EXPECT_FALSE(str("abccba").contains_only("cc"));
        EXPECT_FALSE(str("abccba").contains_only(str::one_of("acc")));
        EXPECT_FALSE(str("abccba").contains_only(str::one_not_of("acc")));

        EXPECT_TRUE(str("abccba").contains_only("abccba"));
        EXPECT_TRUE(str("abccba").contains_only(str::one_of("abc")));
        EXPECT_TRUE(str("abccba").contains_only(str::one_not_of("def")));

        EXPECT_FALSE(str("").contains_only("abccba"));
        EXPECT_TRUE(str("").contains_only(str::one_of("abc")));
        EXPECT_TRUE(str("").contains_only(str::one_not_of("def")));
    }

    TEST(TestAuxString, Edit)
    {
        str a1  = "abc"; EXPECT_NO_THROW(a1.insert(0, "_")); EXPECT_EQ(a1, "_abc");
        str a2  = "abc"; EXPECT_NO_THROW(a2.insert(1, "_")); EXPECT_EQ(a2, "a_bc");
        str a3  = "abc"; EXPECT_NO_THROW(a3.insert(2, "_")); EXPECT_EQ(a3, "ab_c");
        str a4  = "abc"; EXPECT_NO_THROW(a4.insert(3, "_")); EXPECT_EQ(a4, "abc_");
    }
}
//expect all
//{
//    text("abc")::starts.with("") = true;
//    text("abc")::starts.with("a") = true;
//    text("abc")::starts.with("ab") = true;
//    text("abc")::starts.with("abcd") = false;
//    text("abc")::starts.with("abd") = false;
//    text("abc")::starts.with("ad") = false;
//    text("abc")::starts.with("d") = false;
//}
