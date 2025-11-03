#pragma once
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define Vec2f glm::vec2
#define Vec3f glm::vec3
#define Vec4f glm::vec4
#define Mat3f glm::mat3
#define Mat4f glm::mat4
#define Quatf glm::quat

#define Vec2fzero glm::vec2(0.0f, 0.0f)
#define Vec3fzero glm::vec3(0.0f, 0.0f, 0.0f)
#define Vec4fzero glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
#define Mat3fidentity glm::mat3(1.0f)
#define Mat4fidentity glm::mat4(1.0f)
#define Quatfidentity glm::quat(1.0f, 0.0f, 0.0f, 0.0f)

namespace pixl::core::maths
{
    constexpr float PI = 3.14159265358979323846f;

    class Maths
    {
    public:
        static int Init();
        static int Quit();

        //***************************************************//
        //***************** Trigonometry ********************//
        //**************************************************//

        static inline constexpr float DegToRad(float deg) noexcept { return deg * (PI / 180.0f); }
        static inline constexpr float RadToDeg(float rad) noexcept { return rad * (180.0f / PI); }

        static inline constexpr float Sin(float v) noexcept { return std::sin(v); }
        static inline constexpr float Cos(float v) noexcept { return std::cos(v); }
        static inline constexpr float Tan(float v) noexcept { return std::tan(v); }
        static inline constexpr float Asin(float v) noexcept { return std::asin(v); }
        static inline constexpr float Acos(float v) noexcept { return std::acos(v); }
        static inline constexpr float Atan(float v) noexcept { return std::atan(v); }
        static inline constexpr float Atan2(float y, float x) noexcept { return std::atan2(y, x); }

        //***************************************************//
        //* Normalization, Clamping, Lerp, SmoothStep, Sign *//
        //***************************************************//

        template <typename T>
        static inline constexpr T Clamp(T v, T min, T max) noexcept
        {
            return (v < min) ? min : (v > max) ? max
                                               : v;
        }

        template <typename T>
        static inline constexpr T Saturate(T v) noexcept
        {
            return Clamp(v, T(0), T(1));
        }

        template <typename T>
        static inline constexpr T Lerp(T a, T b, T t) noexcept
        {
            return a + (b - a) * t;
        }

        template <typename T>
        static inline constexpr T SmoothStep(T edge0, T edge1, T x) noexcept
        {
            T t = Saturate((x - edge0) / (edge1 - edge0));
            return t * t * (3 - 2 * t);
        }

        template <typename T>
        static inline constexpr T Sign(T v) noexcept
        {
            return (v > 0) - (v < 0);
        }

        // Common Maths

        static inline constexpr float Floor(float v) noexcept { return std::floor(v); }
        static inline constexpr float Ceil(float v) noexcept { return std::ceil(v); }
        static inline constexpr float Round(float v) noexcept { return std::round(v); }
        static inline constexpr float Frac(float v) noexcept { return v - std::floor(v); }
        static inline constexpr float Sqrt(float v) noexcept { return std::sqrt(v); }
        static inline constexpr float Pow(float a, float b) noexcept { return std::pow(a, b); }
        static inline constexpr float Exp(float v) noexcept { return std::exp(v); }
        static inline constexpr float Log(float v) noexcept { return std::log(v); }
        static inline constexpr float Abs(float v) noexcept { return std::fabs(v); }

        // Vexctor operations

        static inline float Dot(const Vec3f &a, const Vec3f &b) noexcept { return glm::dot(a, b); }
        static inline Vec3f Cross(const Vec3f &a, const Vec3f &b) noexcept { return glm::cross(a, b); }

        static inline float Length(const Vec3f &v) noexcept { return glm::length(v); }
        static inline Vec3f Normalize(const Vec3f &v) noexcept { return glm::normalize(v); }

        static inline float Distance(const Vec3f &a, const Vec3f &b) noexcept { return glm::distance(a, b); }

        static inline Vec3f Reflect(const Vec3f &I, const Vec3f &N) noexcept { return glm::reflect(I, N); }
        static inline Vec3f Refract(const Vec3f &I, const Vec3f &N, float eta) noexcept { return glm::refract(I, N, eta); }

        static inline Vec3f Lerp(const Vec3f &a, const Vec3f &b, float t) noexcept { return a + (b - a) * t; }

        static inline float AngleBetween(const Vec3f &a, const Vec3f &b) noexcept
        {
            float dot = glm::dot(glm::normalize(a), glm::normalize(b));
            return std::acos(Clamp(dot, -1.0f, 1.0f));
        }

        // Matrix operations

        static inline Mat4f Translate(const Vec3f &t) noexcept { return glm::translate(Mat4fidentity, t); }
        static inline Mat4f Scale(const Vec3f &s) noexcept { return glm::scale(Mat4fidentity, s); }
        static inline Mat4f Rotate(float angleRad, const Vec3f &axis) noexcept { return glm::rotate(Mat4fidentity, angleRad, axis); }

        static inline Mat4f LookAt(const Vec3f &eye, const Vec3f &center, const Vec3f &up) noexcept
        {
            return glm::lookAt(eye, center, up);
        }

        static inline Mat4f Perspective(float fovY, float aspect, float nearZ, float farZ) noexcept
        {
            return glm::perspective(fovY, aspect, nearZ, farZ);
        }

        static inline Mat4f Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
        {
            return glm::ortho(left, right, bottom, top, nearZ, farZ);
        }

    private:
        static bool IsInitialized_;
    };

}
