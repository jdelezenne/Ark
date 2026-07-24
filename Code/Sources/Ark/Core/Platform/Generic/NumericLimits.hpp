#pragma once

#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"

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
            return 255;
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
            return 65535;
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
            return 4294967295U;
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
            return 18446744073709551615ULL;
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
            return -128;
        }

        static constexpr int8 max()
        {
            return 127;
        }

        static constexpr int8 lowest()
        {
            return -128;
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
            return -32768;
        }

        static constexpr int16 max()
        {
            return 32767;
        }

        static constexpr int16 lowest()
        {
            return -32768;
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
            return -2147483648;
        }

        static constexpr int32 max()
        {
            return 2147483647;
        }

        static constexpr int32 lowest()
        {
            return -2147483648;
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
            return -9223372036854775807LL - 1;
        }

        static constexpr int64 max()
        {
            return 9223372036854775807LL;
        }

        static constexpr int64 lowest()
        {
            return -9223372036854775807LL - 1;
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
            return 1.175494351e-38F;
        }

        static constexpr float32 max()
        {
            return 3.402823466e+38F;
        }

        static constexpr float32 lowest()
        {
            return -3.402823466e+38F;
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
            return 1.192092896e-07F;
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
            return 2.2250738585072014e-308;
        }

        static constexpr float64 max()
        {
            return 1.7976931348623157e+308;
        }

        static constexpr float64 lowest()
        {
            return -1.7976931348623157e+308;
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
            return 2.2204460492503131e-016;
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
