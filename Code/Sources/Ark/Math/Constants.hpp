#pragma once

#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Math/Types.hpp"

namespace Ark::Math
{
    namespace Internal
    {
        /// Represents the value of pi (π).
        template <typename T>
        constexpr T Pi = T(3.14159265358979323846L);

        /// Represents the value of pi times two (2π).
        template <typename T>
        constexpr T TwoPi = T(6.28318530717958647692L);

        /// Represents the value of one over pi (1/π).
        template <typename T>
        constexpr T OneOverPi = T(0.31830988618379067154L);

        /// Represents the value of one over pi times two (1/2π).
        template <typename T>
        constexpr T OneOver2Pi = T(0.15915494309189533576L);

        /// Represents the value of pi divided by two (π/2).
        template <typename T>
        constexpr T PiOver2 = T(1.57079632679489661923L);

        /// Represents the value of pi divided by four (π/4).
        template <typename T>
        constexpr T PiOver4 = T(0.78539816339744830962L);

        /// Represents the machine epsilon.
        template <typename T>
        constexpr T Epsilon = NumericLimits<T>::epsilon();
    }

    /// Represents the value of pi (π).
    constexpr float32 Pi = Internal::Pi<float32>;

    /// Represents the value of pi times two (2π).
    constexpr float32 TwoPi = Internal::TwoPi<float32>;

    /// Represents the value of one over pi (1/π).
    constexpr float32 OneOverPi = Internal::OneOverPi<float32>;

    /// Represents the value of one over pi times two (1/2π).
    constexpr float32 OneOver2Pi = Internal::OneOver2Pi<float32>;

    /// Represents the value of pi divided by two (π/2).
    constexpr float32 PiOver2 = Internal::PiOver2<float32>;

    /// Represents the value of pi divided by four (π/4).
    constexpr float32 PiOver4 = Internal::PiOver4<float32>;

    /// Represents the machine epsilon.
    constexpr float32 Epsilon = Internal::Epsilon<float32>;

    template <typename T>
    constexpr T ZeroTolerance = {};

    template <>
    inline constexpr float32 ZeroTolerance<float32> = 1e-6f;

    template <>
    inline constexpr float64 ZeroTolerance<float64> = 1e-16;
}
