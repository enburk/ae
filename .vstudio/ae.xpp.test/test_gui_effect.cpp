#include "pch.h"
#include "../../_xpp/gui_effect.h"
namespace test_gui
{
    TEST(TestGuiEffect, Transition)
    {
        EXPECT_EQ(gui::transition_state<uint8_t>(0, 255,   0,   0), 255);
        EXPECT_EQ(gui::transition_state<uint8_t>(0, 255,   0, 255),   0);
        EXPECT_EQ(gui::transition_state<uint8_t>(0, 255,  64, 255),  64);
        EXPECT_EQ(gui::transition_state<uint8_t>(0, 255, 128, 255), 128);
        EXPECT_EQ(gui::transition_state<uint8_t>(0, 255, 192, 255), 192);
        EXPECT_EQ(gui::transition_state<uint8_t>(0, 255, 255, 255), 255);
        EXPECT_EQ(gui::transition_state<uint8_t>(0, 255, 256, 255), 255);

        EXPECT_EQ(gui::transition_state<bool>(false, true,   0,   0), true );
        EXPECT_EQ(gui::transition_state<bool>(false, true,   0, 255), false);
        EXPECT_EQ(gui::transition_state<bool>(false, true,  64, 255), false);
        EXPECT_EQ(gui::transition_state<bool>(false, true, 128, 255), false);
        EXPECT_EQ(gui::transition_state<bool>(false, true, 192, 255), false);
        EXPECT_EQ(gui::transition_state<bool>(false, true, 255, 255), true );
        EXPECT_EQ(gui::transition_state<bool>(false, true, 256, 255), true );
    }
}