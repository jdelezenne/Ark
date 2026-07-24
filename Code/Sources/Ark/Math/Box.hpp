#pragma once

#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    /// A box in 3D space, defined by an origin point and size.
    /// Similar to BoundingBox but defined by origin and size instead of min/max corners.
    struct Box final
    {
        /// The origin (minimum corner) of the box.
        Vector3 origin;

        /// The size (width, height, depth) of the box.
        Vector3 size;

        static Box const zero;

        /// Constructs a box with default origin and size at zero.
        constexpr Box() = default;

        /// Constructs a box with specified origin and size.
        /// @param origin The origin point (minimum corner).
        /// @param size The size of the box in each dimension.
        constexpr Box(Vector3 const& origin, Vector3 const& size);
    };
}
