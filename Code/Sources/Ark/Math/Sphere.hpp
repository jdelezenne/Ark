#pragma once

#include "Ark/Math/Rect.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    struct BoundingBox;

    /// A sphere in 3D space, defined by a center point and radius.
    /// Used for volume queries, collision detection, and bounding volume operations.
    struct Sphere final
    {
        /// The center point of the sphere.
        Vector3 center;

        /// The radius of the sphere.
        float32 radius;

        /// Constructs a sphere with default center and zero radius.
        constexpr Sphere() = default;

        /// Constructs a sphere with the specified center and radius.
        /// @param center The center point of the sphere.
        /// @param radius The radius of the sphere.
        constexpr Sphere(Vector3 const& center, float32 radius)
            : center{center}
            , radius{radius}
        {
        }

        /// Checks if this sphere contains a point.
        /// @param point The point to test.
        /// @return True if the point is inside or on the sphere, false otherwise.
        bool contains(Vector3 const& point) const;

        /// Checks if this sphere overlaps with another sphere.
        /// @param circle The other sphere to test against.
        /// @return True if the spheres overlap, false otherwise.
        bool overlaps(Sphere const& circle) const;

        /// Expands this sphere by the specified radius.
        /// @param radius The amount to expand (can be negative to shrink).
        /// @return A new expanded sphere.
        Sphere expand(float radius) const;

        /// Gets the axis-aligned bounding box that contains this sphere.
        /// @return The axis-aligned bounding box.
        BoundingBox getAABB() const;

        /// Computes the distance from this sphere's surface to a point.
        /// @param point The point to measure distance to.
        /// @return The distance from the surface to the point (negative if point is inside).
        float32 getDistanceTo(Vector3 const& point) const;

        /// Computes the distance between the surfaces of two spheres.
        /// @param circle The other sphere.
        /// @return The distance between surfaces (negative if spheres overlap).
        float32 getDistanceTo(Sphere const& circle) const;
    };
}
