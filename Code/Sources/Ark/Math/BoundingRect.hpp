#pragma once

#include "Ark/Collections/Slice.hpp"
#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Rect.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector2.hpp"

namespace Ark::Math
{
    /// An axis-aligned bounding rectangle in 2D space.
    /// Defined by minimum and maximum corner points.
    /// Supports containment tests, intersection, union, and transformations.
    struct BoundingRect final
    {
        /// The minimum corner of the bounding rectangle.
        Vector2 minimum{Vector2::Zero};

        /// The maximum corner of the bounding rectangle.
        Vector2 maximum{Vector2::Zero};

        static BoundingRect const Empty;

        /// Constructs a bounding rectangle with default minimum and maximum at origin.
        constexpr BoundingRect() = default;

        /// Constructs a bounding rectangle with specified minimum and maximum corners.
        /// @param minimum The minimum corner point.
        /// @param maximum The maximum corner point.
        constexpr BoundingRect(Vector2 const& minimum, Vector2 const& maximum)
            : minimum{minimum}
            , maximum{maximum}
        {
        }

        /// Checks if this rectangle has valid bounds (min <= max).
        /// @return True if valid, false otherwise.
        constexpr bool isValid() const
        {
            return (minimum.x <= maximum.x) && (minimum.y <= maximum.y);
        }

        /// Checks if this rectangle is empty (zero-sized).
        /// @return True if empty, false otherwise.
        constexpr bool isEmpty() const
        {
            return (minimum.x == maximum.x) && (minimum.y == maximum.y);
        }

        /// Checks if a point is contained within this rectangle.
        /// @param point The point to test.
        /// @return True if the point is inside or on the edge, false otherwise.
        constexpr bool contains(Vector2 const& point) const
        {
            return (point.x >= minimum.x && point.x <= maximum.x) &&
                   (point.y >= minimum.y && point.y <= maximum.y);
        }

        /// Checks if this rectangle overlaps with another.
        /// @param point The other rectangle to test against.
        /// @return True if the rectangles overlap, false otherwise.
        constexpr bool overlaps(BoundingRect const& point) const
        {
            Vector2 const start1 = minimum;
            Vector2 const end1 = maximum;

            Vector2 const start2 = point.minimum;
            Vector2 const end2 = point.maximum;

            return rangeIntersection(start1.x, end1.x, start2.x, end2.x) &&
                   rangeIntersection(start1.y, end1.y, start2.y, end2.y);
        }

        constexpr BoundingRect expand(float32 width, float32 height) const
        {
            return {
                Vector2{minimum.x - width, minimum.y - height},
                Vector2{maximum.x + width, maximum.y + height},
            };
        }

        constexpr BoundingRect expand(float32 amount) const
        {
            return expand(amount, amount);
        }

        BoundingRect expandTo(Vector2 const& point) const
        {
            Vector2 newMinimum = Vector2::min(minimum, point);
            Vector2 newMaximum = Vector2::max(maximum, point);

            return {newMinimum, newMaximum};
        }

        constexpr BoundingRect translate(Vector2 const& translation) const
        {
            return {
                minimum + translation,
                maximum + translation,
            };
        }

        constexpr BoundingRect translate(float32 x, float32 y) const
        {
            return translate(Vector2{x, y});
        }

        constexpr BoundingRect scale(float32 scale) const
        {
            Vector2 const center = (minimum + maximum) * 0.5f;
            Vector2 const halfSize = (maximum - minimum) * 0.5f * scale;

            return {
                center - halfSize,
                center + halfSize,
            };
        }

        constexpr BoundingRect scale(Vector2 const& scale) const
        {
            Vector2 const center = (minimum + maximum) * 0.5f;
            Vector2 const halfSize = (maximum - minimum) * 0.5f;

            return {
                center - halfSize * scale,
                center + halfSize * scale,
            };
        }

        BoundingRect intersectWith(BoundingRect const& other) const
        {
            Vector2 const start1 = minimum;
            Vector2 const end1 = maximum;

            Vector2 const start2 = other.minimum;
            Vector2 const end2 = other.maximum;

            float32 const left = Math::max(start1.x, start2.x);
            float32 const right = Math::min(end1.x, end2.x);
            float32 const top = Math::max(start1.y, start2.y);
            float32 const bottom = Math::min(end1.y, end2.y);

            if (right >= left && bottom >= top)
            {
                return BoundingRect{Vector2{left, top}, Vector2{right, bottom}};
            }
            else
            {
                return BoundingRect::Empty;
            }
        }

        BoundingRect unionWith(BoundingRect const& other) const
        {
            Vector2 const start1 = minimum;
            Vector2 const end1 = maximum;

            Vector2 const start2 = other.minimum;
            Vector2 const end2 = other.maximum;

            float32 const left = Math::min(start1.x, start2.x);
            float32 const right = Math::max(end1.x, end2.x);
            float32 const top = Math::min(start1.y, start2.y);
            float32 const bottom = Math::max(end1.y, end2.y);

            return BoundingRect{Vector2{left, top}, Vector2{right, bottom}};
        }

        static BoundingRect fromPoints(Vector2 const& point1, Vector2 const& point2)
        {
            Vector2 const minimum = Vector2::min(point1, point2);
            Vector2 const maximum = Vector2::max(point1, point2);

            return BoundingRect{minimum, maximum};
        }

        static BoundingRect fromPoints(Collections::Slice<Vector2> const& points)
        {
            if (points.getCount() == 0)
            {
                return BoundingRect::Empty;
            }

            Vector2 minimum = points[0];
            Vector2 maximum = points[0];

            for (Vector2 const& point : points)
            {
                minimum = Vector2::min(minimum, point);
                maximum = Vector2::max(maximum, point);
            }

            return BoundingRect{minimum, maximum};
        }

        static constexpr BoundingRect fromRect(RectFloat const& rect)
        {
            return BoundingRect{
                Vector2{rect.x, rect.y},
                Vector2{rect.x + rect.width, rect.y + rect.height},
            };
        }

        constexpr RectFloat toRect() const
        {
            return RectFloat::fromCorners(minimum.x, minimum.y, maximum.x, maximum.y);
        }

        constexpr bool operator==(BoundingRect const& other) const
        {
            return (minimum == other.minimum && maximum == other.maximum);
        }

        constexpr bool operator!=(BoundingRect const& other) const
        {
            return (minimum != other.minimum || maximum != other.maximum);
        }

        constexpr BoundingRect operator+(BoundingRect const& other) const
        {
            return {
                Vector2::min(minimum, other.minimum),
                Vector2::max(maximum, other.maximum),
            };
        }

        BoundingRect& operator+=(BoundingRect const& other)
        {
            minimum = Vector2::min(minimum, other.minimum);
            maximum = Vector2::max(maximum, other.maximum);
            return *this;
        }

        BoundingRect operator-(BoundingRect const& other) const
        {
            return {
                Vector2::max(minimum, other.minimum),
                Vector2::min(maximum, other.maximum),
            };
        }

        BoundingRect& operator-=(BoundingRect const& other)
        {
            minimum = Vector2::max(minimum, other.minimum);
            maximum = Vector2::min(maximum, other.maximum);
            return *this;
        }

        constexpr BoundingRect operator*(float32 value) const
        {
            return scale(value);
        }

        constexpr BoundingRect& operator*=(float32 value)
        {
            *this = scale(value);
            return *this;
        }

        constexpr BoundingRect operator/(float32 value) const
        {
            return scale(1.0f / value);
        }

        constexpr BoundingRect& operator/=(float32 value)
        {
            *this = scale(1.0f / value);
            return *this;
        }

        constexpr BoundingRect operator*(Vector2 const& value) const
        {
            return scale(value);
        }

        constexpr BoundingRect& operator*=(Vector2 const& value)
        {
            *this = scale(value);
            return *this;
        }

        constexpr BoundingRect operator/(Vector2 const& value) const
        {
            return scale(Vector2{1.0f / value.x, 1.0f / value.y});
        }

        constexpr BoundingRect& operator/=(Vector2 const& value)
        {
            *this = scale(Vector2{1.0f / value.x, 1.0f / value.y});
            return *this;
        }
    };

    inline BoundingRect const BoundingRect::Empty = {Vector2::Zero, Vector2::Zero};
}
