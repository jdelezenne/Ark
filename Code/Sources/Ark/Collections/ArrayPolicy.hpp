#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark::Collections
{
    /// The strategy used to resize a dynamic array.
    enum class ResizeStrategy
    {
        None,  ///< No resizing is allowed (grow or shrink).
        Ratio, ///< Resize by a ratio of the current size.
        Count, ///< Resize by a fixed count.
    };

    /// Concept for a dynamic array policy.
    template <typename T>
    concept ArrayPolicy = requires {
        { T::MinimumCapacity } -> Concepts::ConvertibleTo<usize>;
        { T::Granularity } -> Concepts::ConvertibleTo<usize>;

        { T::GrowStrategy } -> Concepts::ConvertibleTo<ResizeStrategy>;
        { T::GrowRatio } -> Concepts::ConvertibleTo<float>;
        { T::GrowCount } -> Concepts::ConvertibleTo<usize>;

        { T::ShrinkStrategy } -> Concepts::ConvertibleTo<ResizeStrategy>;
        { T::ShrinkRatio } -> Concepts::ConvertibleTo<float>;
        { T::ShrinkCount } -> Concepts::ConvertibleTo<usize>;

        requires(T::GrowStrategy == ResizeStrategy::None) ||
                    ((T::GrowStrategy == ResizeStrategy::Ratio && T::GrowRatio > 1.0f) ||
                     (T::GrowStrategy == ResizeStrategy::Count && T::GrowCount > 0));

        requires(T::ShrinkStrategy == ResizeStrategy::None) ||
                    ((T::ShrinkStrategy == ResizeStrategy::Ratio && T::ShrinkRatio > 0.0f && T::ShrinkRatio < 1.0f) ||
                     (T::ShrinkStrategy == ResizeStrategy::Count && T::ShrinkCount > 0));
    };

    /// The base policy for a resizable dynamic array.
    struct ArrayPolicyType final
    {
        static constexpr usize MinimumCapacity{0}; ///< The minimum capacity of the array.
        static constexpr usize Granularity{0};     ///< The granularity used to grow or shrink the array.

        static constexpr ResizeStrategy GrowStrategy{ResizeStrategy::Ratio}; ///< The strategy used to grow the array.
        static constexpr float32 GrowRatio{1.5f};                            ///< The ratio used to grow the array.
        static constexpr usize GrowCount{0};                                 ///< The count used to grow the array.

        static constexpr ResizeStrategy ShrinkStrategy{ResizeStrategy::Ratio}; ///< The strategy used to shrink the array.
        static constexpr float32 ShrinkRatio{0.75f};                           ///< The ratio used to shrink the array.
        static constexpr usize ShrinkCount{0};                                 ///< The count used to shrink the array.
    };
}
