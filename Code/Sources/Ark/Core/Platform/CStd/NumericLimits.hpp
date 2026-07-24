#pragma once

#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"

#include <float.h>
#include <limits.h>

namespace Ark
{
    template <typename T>
    struct NumericLimits;

    template <>
    struct NumericLimits<uint8> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr uint8 min()
        {
            return 0;
        }

        static constexpr uint8 max()
        {
            return UCHAR_MAX;
        }

        static constexpr uint8 lowest()
        {
            return 0;
        }

        static constexpr bool isInteger()
        {
            return true;
        }

        static constexpr bool isSigned()
        {
            return false;
        }
    };

    template <>
    struct NumericLimits<uint16> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr uint16 min()
        {
            return 0;
        }

        static constexpr uint16 max()
        {
            return USHRT_MAX;
        }

        static constexpr uint16 lowest()
        {
            return 0;
        }

        static constexpr bool isInteger()
        {
            return true;
        }

        static constexpr bool isSigned()
        {
            return false;
        }
    };

    template <>
    struct NumericLimits<uint32> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr uint32 min()
        {
            return 0;
        }

        static constexpr uint32 max()
        {
            return UINT_MAX;
        }

        static constexpr uint32 lowest()
        {
            return 0;
        }

        static constexpr bool isInteger()
        {
            return true;
        }

        static constexpr bool isSigned()
        {
            return false;
        }
    };

    template <>
    struct NumericLimits<uint64> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr uint64 min()
        {
            return 0;
        }

        static constexpr uint64 max()
        {
            return ULLONG_MAX;
        }

        static constexpr uint64 lowest()
        {
            return 0;
        }

        static constexpr bool isInteger()
        {
            return true;
        }

        static constexpr bool isSigned()
        {
            return false;
        }
    };

    template <>
    struct NumericLimits<int8> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr int8 min()
        {
            return SCHAR_MIN;
        }

        static constexpr int8 max()
        {
            return SCHAR_MAX;
        }

        static constexpr int8 lowest()
        {
            return SCHAR_MIN;
        }

        static constexpr bool isInteger()
        {
            return true;
        }

        static constexpr bool isSigned()
        {
            return true;
        }
    };

    template <>
    struct NumericLimits<int16> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr int16 min()
        {
            return SHRT_MIN;
        }

        static constexpr int16 max()
        {
            return SHRT_MAX;
        }

        static constexpr int16 lowest()
        {
            return SHRT_MIN;
        }

        static constexpr bool isInteger()
        {
            return true;
        }

        static constexpr bool isSigned()
        {
            return true;
        }
    };

    template <>
    struct NumericLimits<int32> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr int32 min()
        {
            return INT_MIN;
        }

        static constexpr int32 max()
        {
            return INT_MAX;
        }

        static constexpr int32 lowest()
        {
            return INT_MIN;
        }

        static constexpr bool isInteger()
        {
            return true;
        }

        static constexpr bool isSigned()
        {
            return true;
        }
    };

    template <>
    struct NumericLimits<int64> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr int64 min()
        {
            return LLONG_MIN;
        }

        static constexpr int64 max()
        {
            return LLONG_MAX;
        }

        static constexpr int64 lowest()
        {
            return LLONG_MIN;
        }

        static constexpr bool isInteger()
        {
            return true;
        }

        static constexpr bool isSigned()
        {
            return true;
        }
    };

    template <>
    struct NumericLimits<float32> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr float32 min()
        {
            return FLT_MIN;
        }

        static constexpr float32 max()
        {
            return FLT_MAX;
        }

        static constexpr float32 lowest()
        {
            return -FLT_MAX;
        }

        static constexpr bool isInteger()
        {
            return false;
        }

        static constexpr bool isSigned()
        {
            return true;
        }

        static constexpr float32 epsilon()
        {
            return FLT_EPSILON;
        }

        static constexpr float32 infinity()
        {
            return __builtin_huge_valf();
        }

        static constexpr float32 NaN()
        {
            return __builtin_nanf("0");
        }
    };

    template <>
    struct NumericLimits<float64> final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr float64 min()
        {
            return DBL_MIN;
        }

        static constexpr float64 max()
        {
            return DBL_MAX;
        }

        static constexpr float64 lowest()
        {
            return -DBL_MAX;
        }

        static constexpr bool isInteger()
        {
            return false;
        }

        static constexpr bool isSigned()
        {
            return true;
        }

        static constexpr float64 epsilon()
        {
            return DBL_EPSILON;
        }

        static constexpr float64 infinity()
        {
            return __builtin_huge_val();
        }

        static constexpr float64 NaN()
        {
            return __builtin_nanf("0");
        }
    };
}
