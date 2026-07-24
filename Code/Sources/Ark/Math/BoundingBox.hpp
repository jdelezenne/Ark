#pragma once

#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    /// An axis-aligned bounding box in 3D space.
    /// Defined by minimum and maximum corner points.
    /// Used for volume queries, frustum culling, and collision detection.
    struct BoundingBox final
    {
    public:
        /// The minimum corner of the bounding box.
        Vector3 minimum{Vector3::Zero};
        /// The maximum corner of the bounding box.
        Vector3 maximum{Vector3::Zero};

        /// Constructs a bounding box with default minimum and maximum at origin.
        constexpr BoundingBox() = default;

        /// Constructs a bounding box with specified minimum and maximum corners.
        /// @param minimum The minimum corner point.
        /// @param maximum The maximum corner point.
        constexpr BoundingBox(Vector3 const& minimum, Vector3 const& maximum)
            : minimum{minimum}
            , maximum{maximum}
        {
        }

        /// Checks if this bounding box overlaps with another.
        /// @param point The other bounding box to test against.
        /// @return True if the boxes overlap, false otherwise.
        constexpr bool overlaps(BoundingBox const& point) const
        {
            Vector3 const start1 = minimum;
            Vector3 const end1 = maximum;

            Vector3 const start2 = point.minimum;
            Vector3 const end2 = point.maximum;

            return (rangeIntersection(start1.x, end1.x, start2.x, end2.x) &&
                    rangeIntersection(start1.y, end1.y, start2.y, end2.y) &&
                    rangeIntersection(start1.z, end1.z, start2.z, end2.z));
        }
    };
}
