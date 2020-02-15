#include "pch.h"
#include "../../_xpp/gui_widget.h"
using namespace gui;
namespace test_gui
{
    TEST(TestGuiWidget, CRTP)
    {
        struct A : widget<A> { str s = "A"; };
        struct B : widget<B> { str s = "B"; };
        struct C : widget<C> { str s = "C"; A a; B b; };
        struct D : widget<D> { str s = "D"; B b; C c; A a; };

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

    TEST(TestGuiWidget, Dynamic)
    {
        using std::unique_ptr;
        struct A : widget<A> { str s = "A"; A(base::widget* w = nullptr) : widget<A>(w) {}; };
        struct B : widget<B> { str s = "B"; B(base::widget* w = nullptr) : widget<B>(w) {}; A a; array<unique_ptr<A>> widgets; };
        struct C : widget<C> { str s = "C"; C(base::widget* w = nullptr) : widget<C>(w) {}; array<unique_ptr<B>> widgets; B b; };

        B b; C c; A a;
        ASSERT_EQ(gui::widgets.size(), 3);

        b.widgets += std::make_unique<A>(&b);
        c.widgets += std::make_unique<B>(&c); c.widgets[0]->
          widgets += std::make_unique<A>(c.widgets[0].get());
        ASSERT_EQ(gui::widgets.size(), 6);

        ASSERT_EQ(a.parent, nullptr);
        ASSERT_EQ(b.parent, nullptr);
        ASSERT_EQ(c.parent, nullptr);
        ASSERT_EQ(b.a.parent, &b);
        ASSERT_EQ(c.b.parent, &c);

        ASSERT_EQ(b.children.size(), 2);
        ASSERT_EQ(b.children[0]->parent, &b);
        ASSERT_EQ(b.children[1]->parent, &b);
        ASSERT_EQ(b.widgets.size(), 1);
        ASSERT_EQ(b.widgets[0]->parent, &b);

        ASSERT_EQ(c.children.size(), 2);
        ASSERT_EQ(c.children[0]->parent, &c);
        ASSERT_EQ(c.children[1]->parent, &c);
        ASSERT_EQ(c.widgets.size(), 1);
        ASSERT_EQ(c.widgets[0]->parent, &c);
        ASSERT_EQ(c.widgets[0]->children.size(), 2);
        ASSERT_EQ(c.widgets[0]->children[0]->parent, c.widgets[0].get());
        ASSERT_EQ(c.widgets[0]->children[1]->parent, c.widgets[0].get());

        c.widgets.clear();
        ASSERT_EQ(gui::widgets.size(), 4);
        ASSERT_EQ(c.children.size(), 1);
        ASSERT_EQ(c.children[0]->parent, &c);
    }
}