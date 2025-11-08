#include <gtest/gtest.h>

#include <core/maths/Maths.hpp>

using namespace pixl::core::maths;

namespace
{
    class MathsTest : public ::testing::Test
    {
    protected:
        void SetUp() override { Maths::Init(); }
        void TearDown() override { Maths::Quit(); }
    };
}

TEST_F(MathsTest, DegToRadAndBack)
{
    constexpr float degrees = 90.0f;
    const float radians = Maths::DegToRad(degrees);
    EXPECT_NEAR(radians, pixl::core::maths::PI / 2.0f, 1e-6f);
    EXPECT_NEAR(Maths::RadToDeg(radians), degrees, 1e-5f);
}

TEST_F(MathsTest, ClampSaturateAndSmoothStep)
{
    EXPECT_EQ(Maths::Clamp(5, 0, 3), 3);
    EXPECT_FLOAT_EQ(Maths::Saturate(2.5f), 1.0f);

    const float smooth = Maths::SmoothStep(0.0f, 1.0f, 0.5f);
    EXPECT_NEAR(smooth, 0.5f, 1e-5f);
}

TEST_F(MathsTest, VectorOperations)
{
    const Vec3f a(1.f, 0.f, 0.f);
    const Vec3f b(0.f, 1.f, 0.f);

    EXPECT_FLOAT_EQ(Maths::Dot(a, b), 0.0f);
    const Vec3f cross = Maths::Cross(a, b);
    EXPECT_FLOAT_EQ(cross.x, 0.0f);
    EXPECT_FLOAT_EQ(cross.y, 0.0f);
    EXPECT_FLOAT_EQ(cross.z, 1.0f);

    const float angle = Maths::AngleBetween(a, b);
    EXPECT_NEAR(angle, pixl::core::maths::PI / 2.0f, 1e-6f);
}

TEST_F(MathsTest, MatrixLookAtAndPerspective)
{
    const Vec3f eye(0.f, 0.f, 5.f);
    const Vec3f target(0.f, 0.f, 0.f);
    const Vec3f up(0.f, 1.f, 0.f);

    const Mat4f view = Maths::LookAt(eye, target, up);
    EXPECT_NEAR(view[3][0], 0.0f, 1e-6f);
    EXPECT_NEAR(view[3][1], 0.0f, 1e-6f);
    EXPECT_NEAR(view[3][2], -5.0f, 1e-6f);

    const Mat4f proj = Maths::Perspective(Maths::DegToRad(60.0f), 1.0f, 0.1f, 100.0f);
    EXPECT_NEAR(proj[2][2], - (100.0f + 0.1f) / (100.0f - 0.1f), 1e-3f);
}
