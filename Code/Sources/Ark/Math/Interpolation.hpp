#pragma once

#include "Ark/Math/Constants.hpp"
#include "Ark/Math/Intrinsics.hpp"

namespace Ark::Math
{
    /// Linearly interpolates between two values.
    /// @tparam T The value type (scalar, vector, or type supporting +/- operators).
    /// @param a The start value (t=0).
    /// @param b The end value (t=1).
    /// @param t The interpolation parameter, typically between 0 and 1.
    /// @return The interpolated value.
    template <typename T>
    constexpr T lerp(T const& a, T const& b, float32 t)
    {
        return a + (b - a) * t;
    }

    /// Smoothly interpolates between two values using cubic ease-in-out.
    /// @tparam T The value type (scalar, vector, or type supporting +/- operators).
    /// @param a The start value (t=0).
    /// @param b The end value (t=1).
    /// @param t The interpolation parameter, typically between 0 and 1.
    /// @return The smoothly interpolated value.
    template <typename T>
    constexpr T smoothStep(T const& a, T const& b, float32 t)
    {
        t = t * t * (3.0f - 2.0f * t);
        return lerp(a, b, t);
    }

    /// Smoothly interpolates between two values using quintic ease-in-out (Smootherstep).
    /// Provides smoother transitions than smoothStep.
    /// @tparam T The value type (scalar, vector, or type supporting +/- operators).
    /// @param a The start value (t=0).
    /// @param b The end value (t=1).
    /// @param t The interpolation parameter, typically between 0 and 1.
    /// @return The smoothly interpolated value.
    template <typename T>
    constexpr T smootherStep(T const& a, T const& b, float32 t)
    {
        t = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
        return lerp(a, b, t);
    }

    /// Ease-in sine easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInSine(float32 t)
    {
        return 1.0f - cos(t * Pi * 0.5f);
    }

    /// Ease-out sine easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeOutSine(float32 t)
    {
        return sin(t * Pi * 0.5f);
    }

    /// Ease-in-out sine easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInOutSine(float32 t)
    {
        return -(cos(Pi * t) - 1.0f) * 0.5f;
    }

    /// Ease-in quadratic easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInQuad(float32 t)
    {
        return t * t;
    }

    /// Ease-out quadratic easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeOutQuad(float32 t)
    {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    /// Ease-in-out quadratic easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInOutQuad(float32 t)
    {
        return t < 0.5f ? 2.0f * t * t : 1.0f - pow(-2.0f * t + 2.0f, 2.0f) * 0.5f;
    }

    /// Ease-in cubic easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInCubic(float32 t)
    {
        return t * t * t;
    }

    /// Ease-out cubic easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeOutCubic(float32 t)
    {
        return 1.0f - pow(1.0f - t, 3.0f);
    }

    /// Ease-in-out cubic easing function.
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInOutCubic(float32 t)
    {
        return t < 0.5f ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3.0f) * 0.5f;
    }

    /// Ease-in back easing function (overshoot effect).
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInBack(float32 t)
    {
        const float32 c1 = 1.70158f;
        const float32 c3 = c1 + 1.0f;
        return c3 * t * t * t - c1 * t * t;
    }

    /// Ease-out back easing function (overshoot effect).
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeOutBack(float32 t)
    {
        const float32 c1 = 1.70158f;
        const float32 c3 = c1 + 1.0f;
        return 1.0f + c3 * pow(t - 1.0f, 3.0f) + c1 * pow(t - 1.0f, 2.0f);
    }

    /// Ease-in-out back easing function (overshoot effect).
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInOutBack(float32 t)
    {
        const float32 c1 = 1.70158f;
        const float32 c2 = c1 * 1.525f;

        return t < 0.5f
                 ? (pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) * 0.5f
                 : (pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) * 0.5f;
    }

    /// Ease-in elastic easing function (spring effect).
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInElastic(float32 t)
    {
        const float32 c4 = TwoPi / 3.0f;

        return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f
                                            : -pow(2.0f, 10.0f * t - 10.0f) * sin((t * 10.0f - 10.75f) * c4);
    }

    /// Ease-out elastic easing function (spring effect).
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeOutElastic(float32 t)
    {
        const float32 c4 = TwoPi / 3.0f;

        return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f
                                            : pow(2.0f, -10.0f * t) * sin((t * 10.0f - 0.75f) * c4) + 1.0f;
    }

    /// Ease-in-out elastic easing function (spring effect).
    /// @param t The input parameter, typically between 0 and 1.
    /// @return The eased value.
    float32 easeInOutElastic(float32 t)
    {
        const float32 c5 = TwoPi / 4.5f;

        return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f
                            : t < 0.5f      ? -(pow(2.0f, 20.0f * t - 10.0f) * sin((20.0f * t - 11.125f) * c5)) * 0.5f
                                            : (pow(2.0f, -20.0f * t + 10.0f) * sin((20.0f * t - 11.125f) * c5)) * 0.5f + 1.0f;
    }
}
