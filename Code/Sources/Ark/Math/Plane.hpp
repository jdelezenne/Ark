#pragma once

#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    /// A plane in 3D space, defined by a normal vector and distance from origin.
    /// Used for geometric queries and plane-based operations.
    struct Plane final
    {
    public:
        /// The unit normal vector of the plane.
        Vector3 normal{Vector3::Zero};

        /// The signed distance from the origin along the normal.
        float32 distance{0};

        /// Constructs a plane with zero normal and zero distance.
        constexpr Plane() = default;

        /// Constructs a plane from a normal vector and distance.
        /// @param normal The plane's normal vector.
        /// @param distance The signed distance from origin.
        constexpr Plane(Vector3 const& normal, float32 distance)
            : normal{normal}
            , distance{distance}
        {
        }

        /// Constructs a plane from a point and normal vector.
        /// @param point A point on the plane.
        /// @param normal The plane's normal vector.
        inline Plane(Vector3 const& point, Vector3 const& normal)
            : normal{normal}
            , distance{-point.dot(normal)}
        {
        }

        /// Constructs a plane from three points.
        /// @param p0 The first point on the plane.
        /// @param p1 The second point on the plane.
        /// @param p2 The third point on the plane.
        inline Plane(Vector3 const& p0, Vector3 const& p1, Vector3 const& p2)
        {
            normal = (p1 - p0).cross(p2 - p0).getNormalized();
            distance = -p0.dot(normal);
        }

        inline float32 getDistance(Vector3 const& point) const
        {
            return point.dot(normal) + distance;
        }

        inline bool contains(Vector3 const& point) const
        {
            return isZero(getDistance(point));
        }

        inline bool operator==(Plane const& other) const
        {
            return (normal == other.normal) && (distance == other.distance);
        }

        inline bool operator!=(Plane const& other) const
        {
            return (normal != other.normal) || (distance != other.distance);
        }
    };
}
