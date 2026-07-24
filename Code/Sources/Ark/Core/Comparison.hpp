#pragma once

#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"

namespace Ark
{
    /// Represents the result of a comparison operation.
    enum class Ordering
    {
        Less = -1,   ///< Indicates that the first value is smaller.
        Equal = 0,   ///< Indicates that the two values are equal.
        Greater = 1, ///< Indicates that the first value is larger.
    };

    /// Converts an integer value to an Ordering enum.
    inline constexpr Ordering orderingFromInt(int value)
    {
        if (value < 0)
        {
            return Ordering::Less;
        }
        else if (value > 0)
        {
            return Ordering::Greater;
        }
        else
        {
            return Ordering::Equal;
        }
    }
}
