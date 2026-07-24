#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Math/Types.hpp"

#include <cmath>

namespace Ark::Math
{
    template <Concepts::Arithmetic T>
    constexpr T abs(T x)
    {
        return std::abs(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcSin(T x)
    {
        return std::asin(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcCos(T x)
    {
        return std::acos(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcTan(T x)
    {
        return std::atan(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcTan2(T y, T x)
    {
        return std::atan2(y, x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T ceil(T x)
    {
        return std::ceil(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T cos(T angle)
    {
        return std::cos(angle);
    }

    template <Concepts::FloatingPoint T>
    constexpr T exp(T x)
    {
        return std::exp(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T floor(T x)
    {
        return std::floor(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T log(T x)
    {
        return std::log(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T mod(T x, T y)
    {
        return std::fmod(x, y);
    }

    template <Concepts::Arithmetic T1, Concepts::Arithmetic T2>
    constexpr T1 pow(T1 x, T2 y)
    {
        return std::pow(x, static_cast<T1>(y));
    }

    template <Concepts::FloatingPoint T>
    constexpr T round(T x)
    {
        return std::round(x);
    }

    template <Concepts::Arithmetic T>
    constexpr int sign(T x)
    {
        if (x < 0)
            return -1;
        else
            return 1;
    }

    template <Concepts::FloatingPoint T>
    constexpr T sin(T angle)
    {
        return std::sin(angle);
    }

    template <Concepts::FloatingPoint T>
    constexpr void sinCos(T angle, T& outSin, T& outCos)
    {
        outSin = std::sin(angle);
        outCos = std::cos(angle);
    }

    template <Concepts::FloatingPoint T>
    constexpr T sqrt(T x)
    {
        return std::sqrt(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T cbrt(T x)
    {
        return std::cbrt(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T tan(T angle)
    {
        return std::tan(angle);
    }

    template <Concepts::FloatingPoint T>
    constexpr T square(T x)
    {
        return (x * x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T squareRoot(T x)
    {
        return std::sqrt(x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T inverseSquareRoot(T x)
    {
        return T(1.0) / squareRoot(x);
    }

    template <typename T1, typename T2>
    constexpr T1 min(T1 a, T2 b)
    {
        return a < static_cast<T1>(b) ? a : static_cast<T1>(b);
    }

    template <typename T1, typename T2>
    constexpr T1 max(T1 a, T2 b)
    {
        return a > static_cast<T1>(b) ? a : static_cast<T1>(b);
    }

    template <typename T1, typename T2, typename T3>
    constexpr T1 clamp(T1 value, T2 min, T3 max)
    {
        return static_cast<T1>(Math::max(min, Math::min(value, max)));
    }
}
