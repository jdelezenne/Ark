#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Macros.hpp"

namespace Ark
{
    /// Inclusive numeric range.
    /// @tparam T Arithmetic value type.
    template <Concepts::Arithmetic T>
    struct Range final
    {
        T min{}; ///< Lower bound.
        T max{}; ///< Upper bound.

        /// Constructs an empty range.
        constexpr Range() = default;

        /// Constructs a range from bounds.
        /// @param min Lower bound.
        /// @param max Upper bound.
        constexpr Range(T min, T max)
            : min{min}
            , max{max}
        {
        }

        /// Checks whether a value is inside the range.
        /// @param value Value to test.
        /// @return `true` when `value` lies in [`min`, `max`].
        constexpr bool contains(T value) const
        {
            return value >= min && value <= max;
        }

        /// Clamps a value to the range.
        /// @param value Value to clamp.
        /// @return `value` constrained to [`min`, `max`].
        constexpr T clamp(T value) const
        {
            if (value < min)
            {
                return min;
            }
            if (value > max)
            {
                return max;
            }
            return value;
        }

        /// Returns range length.
        /// @return Difference between max and min.
        constexpr T getLength() const
        {
            return max - min;
        }

        /// Checks whether the range has no extent.
        /// @return `true` when `min` is greater than or equal to `max`.
        constexpr bool isEmpty() const
        {
            return min >= max;
        }

        constexpr bool operator==(Range const& other) const
        {
            return min == other.min && max == other.max;
        }

        constexpr bool operator!=(Range const& other) const
        {
            return !(*this == other);
        }
    };

    /// Range specialization for signed integers.
    using RangeInt = Range<int32>;

    /// Range specialization for unsigned integers.
    using RangeUInt = Range<uint32>;

    /// Range specialization for single-precision floating point.
    using RangeFloat = Range<float32>;

    /// Range specialization for double-precision floating point.
    using RangeDouble = Range<float64>;
}
