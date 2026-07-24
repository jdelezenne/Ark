#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Math/Constants.hpp"
#include "Ark/Math/Intrinsics.hpp"
#include "Ark/Math/Types.hpp"

namespace Ark::Math
{
    /// Clamps the specified value within the range of 0 to 1.
    /// @param value The value to saturate.
    /// @return The specified parameter clamped within the range of 0 to 1.
    template <typename T>
    constexpr T saturate(T value)
    {
        return clamp(value, T(0), T(1));
    }

    /// Clamps the specified value within the range of 0 to 1, wrapping around if necessary.
    /// @param value The value to saturate.
    /// @return The specified parameter clamped within the range of 0 to 1, wrapping around if necessary.
    template <typename T1, typename T2, typename T3>
    constexpr T1 clampWrap(T1 value, T2 min, T3 max)
    {
        T1 range = (static_cast<T1>(max) - static_cast<T1>(min) + T1(1));
        return static_cast<T1>(((value - static_cast<T1>(min)) % range + range) % range + static_cast<T1>(min));
    }

    /// Clamps the specified value within the range of 0 to 1, wrapping around if necessary.
    /// @param value The value to saturate.
    /// @return The specified parameter clamped within the range of 0 to 1, wrapping around if necessary.
    template <typename T>
    constexpr T saturateWrap(T value)
    {
        if (value >= T(0) && value <= T(1))
        {
            return value;
        }
        else
        {
            return value - Math::floor(value);
        }
    }

    constexpr bool isInfinity(float32 x)
    {
        return x == NumericLimits<float32>::infinity();
    }

    constexpr bool isInfinity(float64 x)
    {
        return x == NumericLimits<float64>::infinity();
    }

    /// Checks if a value is zero within a specified tolerance.
    /// @param x The value to check.
    /// @return true if the value is considered zero; otherwise, false.
    inline bool isZero(float32 x)
    {
        return abs(x) < ZeroTolerance<float32>;
    }

    /// Checks if a value is zero within a specified tolerance.
    /// @param x The value to check.
    /// @return true if the value is considered zero; otherwise, false.
    inline bool isZero(float64 x)
    {
        return abs(x) < ZeroTolerance<float64>;
    }

    /// Checks if a value is nearly zero within a specified tolerance.
    /// @param x The value to check.
    /// @param epsilon The tolerance value.
    /// @return true if the value is nearly zero; otherwise, false.
    inline bool isNearZero(float32 x, float32 epsilon = ZeroTolerance<float32>)
    {
        return abs(x) < epsilon;
    }

    /// Checks if a value is nearly zero within a specified tolerance.
    /// @param x The value to check.
    /// @param epsilon The tolerance value.
    /// @return true if the value is nearly zero; otherwise, false.
    inline bool isNearZero(float64 x, float64 epsilon = ZeroTolerance<float64>)
    {
        return abs(x) < epsilon;
    }

    /// Checks if two values are nearly equal within a specified tolerance.
    /// @param a The first value to compare.
    /// @param b The second value to compare.
    /// @param epsilon The tolerance value.
    /// @return true if the values are nearly equal; otherwise, false.
    static bool isNearEqual(float32 a, float32 b, float32 epsilon = ZeroTolerance<float32>)
    {
        return abs(a - b) < epsilon;
    }

    /// Checks if two values are nearly equal within a specified tolerance.
    /// @param a The first value to compare.
    /// @param b The second value to compare.
    /// @param epsilon The tolerance value.
    /// @return true if the values are nearly equal; otherwise, false.
    static bool isNearEqual(float64 a, float64 b, float64 epsilon = ZeroTolerance<float64>)
    {
        return abs(a - b) < epsilon;
    }

    /// Indicates whether a given value is the range specified by a mininum and maximum value.
    /// @param value The value to check.
    /// @return true if the value is in the specified range; otherwise, false.
    template <typename T1, typename T2, typename T3>
    constexpr bool isInRange(T1 value, T2 min, T3 max)
    {
        return (value >= min) && (value <= max);
    }

    template <Concepts::Integral T>
    constexpr T modulo(T a, T b)
    {
        return (a % b);
    }

    template <Concepts::SignedIntegral T>
    constexpr T modulo(T a, T b)
    {
        T result = (a % b);

        if (result < 0)
        {
            result += b;
        }

        return result;
    }

    template <Concepts::FloatingPoint T>
    constexpr T modulo(T a, T b)
    {
        if (b == 0)
        {
            return a;
        }

        return a - b * floor(a / b);
    }

    template <typename T>
    constexpr T degreesToRadians(T angle)
    {
        return angle * Internal::Pi<T> / static_cast<T>(180);
    }

    template <typename T>
    constexpr T radiansToDegrees(T angle)
    {
        return angle * static_cast<T>(180) * Internal::OneOverPi<T>;
    }

    template <typename T>
    constexpr void normalizeAngleTwoPi(T& angle)
    {
        while (angle < static_cast<T>(0.0))
        {
            angle += Internal::TwoPi<T>;
        }

        while (angle >= Internal::TwoPi<T>)
        {
            angle -= Internal::TwoPi<T>;
        }
    }

    template <typename T>
    constexpr void normalizeAnglePi(T& angle)
    {
        while (angle < -Internal::Pi<T>)
        {
            angle += Internal::TwoPi<T>;
        }

        while (angle > Internal::Pi<T>)
        {
            angle -= Internal::TwoPi<T>;
        }
    }

    template <typename T>
    static T square(const T x)
    {
        return x * x;
    }

    inline float64 pow10(int exp)
    {
        float64 result = 1.0;
        float64 base = 10.0;

        const bool isNegative = exp < 0;
        exp = isNegative ? -exp : exp;

        while (exp)
        {
            if (exp & 1)
            {
                result *= base;
            }

            base *= base;
            exp >>= 1;
        }

        return isNegative ? 1.0 / result : result;
    }

    /// Calculate the linear interpolation between two values.
    template <typename T1, typename T2>
    static T1 lerp(T1 const& a, T1 const& b, const T2& alpha)
    {
        return static_cast<T1>(a + alpha * (b - a));
    }

    static float smoothStep(float32 amount)
    {
        return (amount <= 0) ? 0 : amount >= 1 ? 1
                                               : amount * amount * (3 - 2 * amount);
    }

    /// Normalize a value within a range.
    template <typename T>
    static T normalize(T const& value, T const& start, T const& end)
    {
        return (value - start) / (end - start);
    }

    /// Remap a value from an input range to an output range.
    template <typename T>
    T remap(T value, T inputStart, T inputEnd, T outputStart, T outputEnd)
    {
        return (value - inputStart) / (inputEnd - inputStart) * (outputEnd - outputStart) + outputStart;
    }

    /// Wrap a value around a minimum and maximum value.
    template <typename T>
    T wrap(T value, T min, T max)
    {
        return (value - (max - min) * Math::floor((value - min) / (max - min)));
    }

    template <typename T>
    constexpr inline bool rangeIntersection(T start1, T end1, T start2, T end2)
    {
        return (start1 < end2) && (start2 < end1);
    }
}
