#include "pch.h"
#include "../../library/cpp/aux_array.h"
namespace aux
{
    TEST(TestAuxArray, Append)
    {
        array<std::string> a, b, c;
        a += "1";             ASSERT_EQ(a.size(), 1) << "1";
        b += a;               ASSERT_EQ(b.size(), 1) << "2"; ASSERT_EQ(a.size(), 1) << "2";
        b += b;               ASSERT_EQ(b.size(), 2) << "3"; ASSERT_EQ(a.size(), 1) << "3";
        b += std::move(a[0]); ASSERT_EQ(b.size(), 3) << "4"; ASSERT_EQ(a.size(), 1) << "4"; ASSERT_EQ(a[0], "");
        b += "b";             ASSERT_EQ(b.size(), 4) << "5"; ASSERT_EQ(a.size(), 1) << "5";
        c += std::move(b);    ASSERT_EQ(c.size(), 4) << "6"; ASSERT_EQ(b.size(), 4) << "6"; ASSERT_EQ(b[0], ""); ASSERT_EQ(b[3], "");
        c += a;               ASSERT_EQ(c.size(), 5) << "7"; ASSERT_EQ(a.size(), 1) << "7";
    }

    TEST(TestAuxArray, Range)
    {
        aux::array<int> a = {1, 2, 3};

        ASSERT_EQ(a.whole().length, 3);
        ASSERT_EQ(a.from(0).length, 3);
        ASSERT_EQ(a.from(1).length, 2);
        ASSERT_EQ(a.from(2).length, 1);
        ASSERT_EQ(a.from(3).length, 0);
        ASSERT_EQ(a.from(0).upto(0).length, 0);
        ASSERT_EQ(a.from(0).upto(1).length, 1);
        ASSERT_EQ(a.from(0).upto(2).length, 2);
        ASSERT_EQ(a.from(0).upto(3).length, 3);
        ASSERT_EQ(a.from(1).upto(1).length, 0);
        ASSERT_EQ(a.from(1).upto(2).length, 1);
        ASSERT_EQ(a.from(1).upto(3).length, 2);
        ASSERT_EQ(a.from(2).upto(2).length, 0);
        ASSERT_EQ(a.from(2).upto(3).length, 1);
        ASSERT_EQ(a.from(3).upto(3).length, 0);

        int sum;
        sum = 0; for (int n : a) sum += n; ASSERT_EQ(sum, 6);
        sum = 0; for (auto [n,i] : a.whole()) sum += n; ASSERT_EQ(sum, 6);
        sum = 0; for (auto [n,i] : a.from(1)) sum += n; ASSERT_EQ(sum, 5);
        sum = 0; for (auto [n,i] : a.from(1).upto(2)) sum += n; ASSERT_EQ(sum, 2);

        a.from(1).upto(2).erase(); // a = {1, 3};

        sum = 0; for (auto [n,i] : a.whole()) sum += n; ASSERT_EQ(sum, 4);
        sum = 0; for (auto [n,i] : a.from(1)) sum += n; ASSERT_EQ(sum, 3);
        sum = 0; for (auto [n,i] : a.from(1).upto(2)) sum += n; ASSERT_EQ(sum, 3);

        a.insert(1, aux::array{2}); // a = {1, 2, 3};

        sum = 0; for (auto [n,i] : a.whole()) sum += n; ASSERT_EQ(sum, 6);
        sum = 0; for (auto [n,i] : a.from(1)) sum += n; ASSERT_EQ(sum, 5);
        sum = 0; for (auto [n,i] : a.from(1).upto(2)) sum += n; ASSERT_EQ(sum, 2);
    }

    TEST(TestAuxArray, Replace)
    {
        array<int> a = {1, 2, 3};
        array<int> b = {4, 5, 6};
        
        a.upto(0).replace_by(b.from(3)); ASSERT_EQ(a, (array{1, 2, 3}));
        a.upto(1).replace_by(b.upto(1)); ASSERT_EQ(a, (array{4, 2, 3}));
        a.upto(1).replace_by(b.from(1)); ASSERT_EQ(a, (array{5, 6, 2, 3}));
        a.upto(2).replace_by(b.from(2)); ASSERT_EQ(a, (array{6, 2, 3}));
        a.upto(3).replace_by(b.whole()); ASSERT_EQ(a, (array{4, 5, 6}));
    }

    TEST(TestAuxArray, RangeWithEnding)
    {
        //array<char> a = {'a', 'b', 'c'};
        //array<char> b = {'a', 'b', 'c', 'd'};
        //
        //ASSERT_EQ((a.range() + 'd').size(), 4);
        //
        //long l = 0; for (auto [c, n] : a.range() + 'd')
        //{
        //    ASSERT_GE(n, 0);
        //    ASSERT_LE(n, 3);
        //    EXPECT_EQ(c, b[n]);
        //    EXPECT_EQ(n, l++);
        //}
        //
        //ASSERT_EQ((a.range(0, 0) + 'd').size(), 1);
        //
        //for (auto [c, n] : a.range(0, 0) + 'd')
        //{
        //    EXPECT_EQ(c, 'd');
        //    EXPECT_EQ(n,  0 );
        //}
    }
}
