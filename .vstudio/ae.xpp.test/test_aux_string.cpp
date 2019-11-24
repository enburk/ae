#include "pch.h"
#include "../../_xpp/aux_string.h"
namespace test_aux
{
    TEST(TestAuxString, Substr)
    {
        ASSERT_EQ(str("").substr(0,0), "");
        ASSERT_EQ(str("").substr(0,1), "");
        ASSERT_EQ(str("").substr(1,1), "");

        EXPECT_EQ(str("abc").substr(1,0), "");
        EXPECT_EQ(str("abc").substr(1,1), "b");
        EXPECT_EQ(str("abc").substr(1,2), "bc");
        EXPECT_EQ(str("abc").substr(1,3), "bc");

        EXPECT_EQ(str("abc").head(2), "ab");
        EXPECT_EQ(str("abc").till(2), "ab");
        EXPECT_EQ(str("abc").from(2), "c");
        EXPECT_EQ(str("abc").tail(2), "bc");

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

        EXPECT_EQ(str("abccba").find("b", str::start_from(1)).pos, 1);
        EXPECT_EQ(str("abccba").find("b", str::start_from(2)).pos, 4);
        EXPECT_EQ(str("abccba").find("b", str::start_from(5)).size, 0);
        EXPECT_EQ(str("abccba").find("b", str::start_from_end()).pos, 4);
        EXPECT_EQ(str("abccba").find("b", str::start_from_end(1)).pos, 4);
        EXPECT_EQ(str("abccba").find("b", str::start_from_end(2)).pos, 1);
        EXPECT_EQ(str("abccba").find("b", str::start_from_end(5)).size, 0);
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
        EXPECT_EQ(str("abc").insert(0, "_"), "_abc");
        EXPECT_EQ(str("abc").insert(1, "_"), "a_bc");
        EXPECT_EQ(str("abc").insert(2, "_"), "ab_c");
        EXPECT_EQ(str("abc").insert(3, "_"), "abc_");

    }
}
