#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Math/Types.hpp"

namespace Ark::Math
{
    namespace Internal
    {
        template <Concepts::FloatingPoint T>
        constexpr T builtinAbs(T x)
        {
            if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
            {
                return __builtin_fabsf(x);
            }
            else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
            {
                return __builtin_fabsl(x);
            }
            else
            {
                return static_cast<T>(__builtin_fabs(static_cast<double>(x)));
            }
        }
    }

    /// Absolute value without pulling the C/C++ math standard library.
    template <Concepts::Arithmetic T>
    constexpr T abs(T x)
    {
        if constexpr (Traits::isFloatingPoint<T>)
        {
            return Internal::builtinAbs(x);
        }
        else
        {
            return x < T(0) ? -x : x;
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcSin(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_asinf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_asinl(x);
        }
        else
        {
            return static_cast<T>(__builtin_asin(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcCos(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_acosf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_acosl(x);
        }
        else
        {
            return static_cast<T>(__builtin_acos(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcTan(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_atanf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_atanl(x);
        }
        else
        {
            return static_cast<T>(__builtin_atan(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T arcTan2(T y, T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_atan2f(y, x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_atan2l(y, x);
        }
        else
        {
            return static_cast<T>(__builtin_atan2(static_cast<double>(y), static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T ceil(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_ceilf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_ceill(x);
        }
        else
        {
            return static_cast<T>(__builtin_ceil(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T cos(T angle)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_cosf(angle);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_cosl(angle);
        }
        else
        {
            return static_cast<T>(__builtin_cos(static_cast<double>(angle)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T exp(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_expf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_expl(x);
        }
        else
        {
            return static_cast<T>(__builtin_exp(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T floor(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_floorf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_floorl(x);
        }
        else
        {
            return static_cast<T>(__builtin_floor(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T log(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_logf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_logl(x);
        }
        else
        {
            return static_cast<T>(__builtin_log(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T mod(T x, T y)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_fmodf(x, y);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_fmodl(x, y);
        }
        else
        {
            return static_cast<T>(__builtin_fmod(static_cast<double>(x), static_cast<double>(y)));
        }
    }

    template <Concepts::Arithmetic T1, Concepts::Arithmetic T2>
    constexpr T1 pow(T1 x, T2 y)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T1>, float>)
        {
            return __builtin_powf(static_cast<float>(x), static_cast<float>(y));
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T1>, long double>)
        {
            return static_cast<T1>(__builtin_powl(static_cast<long double>(x), static_cast<long double>(y)));
        }
        else
        {
            return static_cast<T1>(__builtin_pow(static_cast<double>(x), static_cast<double>(y)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T round(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_roundf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_roundl(x);
        }
        else
        {
            return static_cast<T>(__builtin_round(static_cast<double>(x)));
        }
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
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_sinf(angle);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_sinl(angle);
        }
        else
        {
            return static_cast<T>(__builtin_sin(static_cast<double>(angle)));
        }
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
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_sqrtf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_sqrtl(x);
        }
        else
        {
            return static_cast<T>(__builtin_sqrt(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T cbrt(T x)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_cbrtf(x);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_cbrtl(x);
        }
        else
        {
            return static_cast<T>(__builtin_cbrt(static_cast<double>(x)));
        }
    }

    template <Concepts::FloatingPoint T>
    constexpr T tan(T angle)
    {
        if constexpr (Traits::isSame<Traits::RemoveCVType<T>, float>)
        {
            return __builtin_tanf(angle);
        }
        else if constexpr (Traits::isSame<Traits::RemoveCVType<T>, long double>)
        {
            return __builtin_tanl(angle);
        }
        else
        {
            return static_cast<T>(__builtin_tan(static_cast<double>(angle)));
        }
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
    constexpr T1 clamp(T1 value, T2 minValue, T3 maxValue)
    {
        return static_cast<T1>(Math::max(minValue, Math::min(value, maxValue)));
    }
}
