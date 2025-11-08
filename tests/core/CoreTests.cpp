#include <gtest/gtest.h>

#include <core/Core.hpp>

using namespace pixl::core;

namespace
{
    class CoreTest : public ::testing::Test
    {
    protected:
        void TearDown() override
        {
            Core::Quit();
        }
    };
}

TEST_F(CoreTest, InitAndQuitReturnSuccess)
{
    CoreInitData initData{};
    initData.enableConsole = false;
    initData.logToFile = false;

    EXPECT_EQ(Core::Init(initData), 0);
    EXPECT_EQ(Core::Quit(), 0);
}

TEST_F(CoreTest, MultipleInitCallsAreIdempotent)
{
    CoreInitData initData{};
    initData.enableConsole = false;
    initData.logToFile = false;

    EXPECT_EQ(Core::Init(initData), 0);
    EXPECT_EQ(Core::Init(initData), 0); // second init should be a no-op
}

TEST(CoreBuildInfoTest, BuildInfoFieldsAreNotNull)
{
    const auto info = Core::GetBuildInfo();
    EXPECT_NE(info.buildDate, nullptr);
    EXPECT_NE(info.buildTime, nullptr);
    EXPECT_NE(info.compiler, nullptr);
    EXPECT_NE(info.buildType, nullptr);
    EXPECT_STRNE(info.buildType, "");
}
