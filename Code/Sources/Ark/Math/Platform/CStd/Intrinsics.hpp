#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Math/Types.hpp"

#include <math.h>

namespace Ark::Math
{
    template <Concepts::Arithmetic T>
    constexpr T abs(T x)
    {
        if constexpr (Traits::isFloatingPoint<T>)
        {
            if constexpr (sizeof(T) == sizeof(float))
            {
                return static_cast<T>(fabsf(static_cast<float>(x)));
            }
            else if constexpr (sizeof(T) == sizeof(long double))
            {
                return static_cast<T>(fabsl(static_cast<long double>(x)));
            }
            else
            {
                return static_cast<T>(fabs(static_cast<double>(x)));
            }
        }
        else
        {
            return x < T(0) ? -x : x;
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcSin(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(asinf(static_cast<float>(x)));
        }
        return static_cast<T>(asin(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcCos(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(acosf(static_cast<float>(x)));
        }
        return static_cast<T>(acos(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcTan(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(atanf(static_cast<float>(x)));
        }
        return static_cast<T>(atan(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcTan2(T y, T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(atan2f(static_cast<float>(y), static_cast<float>(x)));
        }
        return static_cast<T>(atan2(static_cast<double>(y), static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T ceil(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(ceilf(static_cast<float>(x)));
        }
        return static_cast<T>(::ceil(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T cos(T angle)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(cosf(static_cast<float>(angle)));
        }
        return static_cast<T>(::cos(static_cast<double>(angle)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T exp(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(expf(static_cast<float>(x)));
        }
        return static_cast<T>(::exp(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T floor(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(floorf(static_cast<float>(x)));
        }
        return static_cast<T>(::floor(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T log(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(logf(static_cast<float>(x)));
        }
        return static_cast<T>(::log(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T mod(T x, T y)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(fmodf(static_cast<float>(x), static_cast<float>(y)));
        }
        return static_cast<T>(fmod(static_cast<double>(x), static_cast<double>(y)));
    }

    template <Concepts::Arithmetic T1, Concepts::Arithmetic T2>
    constexpr T1 pow(T1 x, T2 y)
    {
        if constexpr (sizeof(T1) == sizeof(float))
        {
            return static_cast<T1>(powf(static_cast<float>(x), static_cast<float>(y)));
        }
        return static_cast<T1>(::pow(static_cast<double>(x), static_cast<double>(y)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T round(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(roundf(static_cast<float>(x)));
        }
        return static_cast<T>(::round(static_cast<double>(x)));
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
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(sinf(static_cast<float>(angle)));
        }
        return static_cast<T>(::sin(static_cast<double>(angle)));
    }

    template <Concepts::FloatingPoint T>
    constexpr void sinCos(T angle, T& outSin, T& outCos)
    {
        outSin = sin(angle);
        outCos = cos(angle);
    }

    template <Concepts::FloatingPoint T>
    constexpr T sqrt(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(sqrtf(static_cast<float>(x)));
        }
        return static_cast<T>(::sqrt(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T cbrt(T x)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(cbrtf(static_cast<float>(x)));
        }
        return static_cast<T>(::cbrt(static_cast<double>(x)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T tan(T angle)
    {
        if constexpr (sizeof(T) == sizeof(float))
        {
            return static_cast<T>(tanf(static_cast<float>(angle)));
        }
        return static_cast<T>(::tan(static_cast<double>(angle)));
    }

    template <Concepts::FloatingPoint T>
    constexpr T square(T x)
    {
        return (x * x);
    }

    template <Concepts::FloatingPoint T>
    constexpr T squareRoot(T x)
    {
        return sqrt(x);
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
