#pragma once

#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"

#include <limits>

namespace Ark
{
    template <typename T>
    struct NumericLimits final
    {
        ARK_STATIC_STRUCT(NumericLimits)

        static constexpr T min()
        {
            return std::numeric_limits<T>::min();
        }

        static constexpr T max()
        {
            return std::numeric_limits<T>::max();
        }

        static constexpr T lowest()
        {
            return std::numeric_limits<T>::lowest();
        }

        static constexpr bool isInteger()
        {
            return std::numeric_limits<T>::is_integer;
        }

        static constexpr bool isSigned()
        {
            return std::numeric_limits<T>::is_signed;
        }

        static constexpr T epsilon()
        {
            return std::numeric_limits<T>::epsilon();
        }

        static constexpr T infinity()
        {
            return std::numeric_limits<T>::infinity();
        }

        static constexpr T NaN()
        {
            return std::numeric_limits<T>::quiet_NaN();
        }
    };
}
