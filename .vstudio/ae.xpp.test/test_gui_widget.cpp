#include "pch.h"
#include "../../_xpp/gui_widget.h"
namespace test_gui
{
    struct A : gui::widget<A> { str s = "A"; };
    struct B : gui::widget<B> { str s = "B"; };
    struct C : gui::widget<C> { str s = "C"; A a; B b; };
    struct D : gui::widget<D> { str s = "D"; B b; C c; A a; };

    TEST(TestGuiWidget, CRTP)
    {
        B b; D d; A a; C c;

        ASSERT_EQ(a.parent, nullptr);
        ASSERT_EQ(b.parent, nullptr);
        ASSERT_EQ(c.parent, nullptr);
        ASSERT_EQ(d.parent, nullptr);
        ASSERT_EQ(c.a.parent, &c);
        ASSERT_EQ(c.b.parent, &c);
        ASSERT_EQ(c.children.size(), 2);
        ASSERT_EQ(c.children[0]->parent, &c);
        ASSERT_EQ(c.children[1]->parent, &c);

        ASSERT_EQ(d.a.parent, &d);
        ASSERT_EQ(d.b.parent, &d);
        ASSERT_EQ(d.c.parent, &d);
        ASSERT_EQ(d.c.a.parent, &d.c);
        ASSERT_EQ(d.c.b.parent, &d.c);
        ASSERT_EQ(d.c.children.size(), 2);
        ASSERT_EQ(d.c.children[0]->parent, &d.c);
        ASSERT_EQ(d.c.children[1]->parent, &d.c);
    }
}