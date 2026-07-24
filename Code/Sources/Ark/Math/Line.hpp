#pragma once

#include "Ark/Core/Option.hpp"
#include "Ark/Math/Rect.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector2.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    static constexpr float32 getDenominator(Vector2 const& p0, Vector2 const& p1, Vector2 const& p2, Vector2 const& p3)
    {
        return ((p1.x - p0.x) * (p3.y - p2.y) - (p1.y - p0.y) * (p3.x - p2.x));
    }

    /// A line segment in 2D or 3D space.
    /// @tparam T The vector type (Vector2 or Vector3).
    /// Used for geometric queries, distance calculations, and intersection tests.
    template <typename T>
    struct Line final
    {
    public:
        /// The starting point of the line segment.
        T start;

        /// The ending point of the line segment.
        T end;

    public:
        /// Constructs a line with default start and end points.
        constexpr Line() = default;

        /// Constructs a line with specified start and end points.
        /// @param start The starting point.
        /// @param end The ending point.
        constexpr Line(T const& start, T const& end)
            : start{start}
            , end{end}
        {
        }

        /// Computes the length of this line segment.
        /// @return The distance from start to end.
        float32 getLength() const
        {
            return (end - start).getLength();
        }

        /// Computes the squared length of this line segment.
        /// @return The squared distance from start to end.
        float32 getLengthSquared() const
        {
            return (end - start).getLengthSquared();
        }

        /// Computes the perpendicular distance from a point to this line.
        /// @param point The point to measure from.
        /// @return The perpendicular distance.
        float32 getDistance(T const& point) const
        {
            T const closestPoint = getClosestPoint(point);
            return (point - closestPoint).getLength();
        }

        /// Finds the closest point on this line segment to a given point.
        /// @param point The reference point.
        /// @return The closest point on the line segment.
        T getClosestPoint(T const& point) const
        {
            T const lineVector = (end - start);
            float32 const lengthSquared = lineVector.getLengthSquared();
            if (lengthSquared <= Math::ZeroTolerance<float32>)
            {
                return start;
            }

            float32 t = (point - start).dot(lineVector) / lengthSquared;
            if (t < 0.0f)
            {
                t = 0.0f;
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
            }

            return start + (lineVector * t);
        }

        /// Checks if a point lies on this line segment (within epsilon tolerance).
        /// @param point The point to test.
        /// @param epsilon The tolerance for the check.
        /// @return True if the point is on the line, false otherwise.
        bool contains(T const& point, float32 epsilon) const
        {
            float32 const epsilonSquared = epsilon * epsilon;
            T const closestPoint = getClosestPoint(point);
            return (point - closestPoint).getLengthSquared() <= epsilonSquared;
        }

        /// Computes the intersection point with another line segment (2D only).
        /// @param other The other line to test against.
        /// @return The intersection point if found, or none if lines don't intersect or are parallel.
        Option<Vector2> intersection(Line const& other) const
        {
            Vector2 const p0 = start;
            Vector2 const p1 = end;
            Vector2 const p2 = other.start;
            Vector2 const p3 = other.end;

            float32 const denominator = getDenominator(p0, p1, p2, p3);

            // Check if the lines are  parallel
            if (Math::abs(denominator) < Math::ZeroTolerance<float32>)
            {
                return none;
            }

            float32 const t = ((p2.x - p0.x) * (p3.y - p2.y) - (p2.y - p0.y) * (p3.x - p2.x)) / denominator;
            float32 const u = -((p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x)) / denominator;

            // Check if the intersection point is within both line segments
            if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f)
            {
                float32 const intersectionX = p0.x + t * (p1.x - p0.x);
                float32 const intersectionY = p0.y + t * (p1.y - p0.y);
                return Vector2{intersectionX, intersectionY};
            }

            return none;
        }
    };

    using Line2 = Line<Vector2>;
    using Line3 = Line<Vector3>;
}
