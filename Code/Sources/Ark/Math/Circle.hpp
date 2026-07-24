#pragma once

#include "Ark/Math/Rect.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector2.hpp"

namespace Ark::Math
{
    /// A circle in 2D space, defined by a center point and radius.
    /// Used for volume queries, collision detection, and bounding area operations.
    struct Circle final
    {
        /// The center point of the circle.
        Vector2 center{Vector2::Zero};
        /// The radius of the circle.
        float32 radius{0};

        /// Constructs a circle with default center and zero radius.
        constexpr Circle() = default;

        /// Constructs a circle with the specified center and radius.
        /// @param center The center point of the circle.
        /// @param radius The radius of the circle.
        constexpr Circle(Vector2 const& center, float32 radius)
            : center{center}
            , radius{radius}
        {
        }

        /// Checks if this circle contains a point.
        /// @param point The point to test.
        /// @return True if the point is inside or on the circle, false otherwise.
        bool contains(Vector2 const& point) const;

        /// Checks if this circle overlaps with another circle.
        /// @param circle The other circle to test against.
        /// @return True if the circles overlap, false otherwise.
        bool overlaps(Circle const& circle) const;

        /// Expands this circle by the specified radius.
        /// @param radius The amount to expand (can be negative to shrink).
        /// @return A new expanded circle.
        Circle expand(float radius) const;

        /// Gets the axis-aligned bounding box that contains this circle.
        /// @return The axis-aligned bounding rectangle.
        RectFloat getAABB() const;

        /// Computes the distance from this circle's edge to a point.
        /// @param point The point to measure distance to.
        /// @return The distance from the edge to the point (negative if point is inside).
        float32 getDistanceTo(Vector2 const& point) const;

        /// Computes the distance between the edges of two circles.
        /// @param circle The other circle.
        /// @return The distance between edges (negative if circles overlap).
        float32 getDistanceTo(Circle const& circle) const;
    };
}
