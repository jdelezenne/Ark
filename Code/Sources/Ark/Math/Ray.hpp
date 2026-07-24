#pragma once

#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    /// A ray in 3D space, defined by a starting position and direction.
    /// Used for ray-casting and geometric queries.
    struct Ray final
    {
        /// The starting position of the ray.
        Vector3 position;

        /// The direction vector of the ray (typically normalized).
        Vector3 direction;

        static Ray const zero;

        /// Constructs a ray with default position and direction (both zero).
        constexpr Ray()
        {
        }

        /// Constructs a ray with the specified position and direction.
        /// @param position The starting position of the ray.
        /// @param direction The direction vector of the ray.
        constexpr Ray(Vector3 const& position, Vector3 const& direction)
            : position{position}
            , direction{direction}
        {
        }
    };
}
