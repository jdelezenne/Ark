#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Collections/Slice.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector2.hpp"

namespace Ark::Math
{
    struct BoundingRect;
    struct Circle;

    /// A polygon defined by an array of 2D vertices.
    /// Supports operations like translation, rotation, scaling, and geometric queries.
    struct Polygon final
    {
    private:
        Collections::Array<Vector2> points;

    public:
        /// Returns the number of vertices in this polygon.
        /// @return The vertex count.
        usize getPointCount() const;

        /// Gets a read-only view of the polygon's vertices.
        /// @return A slice of all vertices.
        Collections::Slice<Vector2 const> getPoints() const;

        /// Sets the polygon's vertices from a slice.
        /// @param points The new vertices to use.
        void setPoints(Collections::Slice<Vector2> const& points);

        /// Clears all vertices from this polygon.
        void clear();

        /// Resizes the polygon to have the specified number of vertices.
        /// @param pointCount The new vertex count.
        void resize(usize pointCount);

        /// Appends a vertex to this polygon.
        /// @param point The vertex to append.
        void append(Vector2 const& point);

        /// Reverses the winding order of the vertices.
        void invert();

        /// Computes the geometric center (centroid) of this polygon.
        /// @return The center point.
        Vector2 getCentre() const;

        /// Gets the axis-aligned bounding rectangle of this polygon.
        /// @return The bounding rectangle.
        BoundingRect getBoundingRect() const;

        /// Gets the smallest bounding circle that contains this polygon.
        /// @return The bounding circle.
        Circle getBoundingCircle() const;

        /// Translates (moves) this polygon by a given offset.
        /// @param offset The translation vector.
        void translate(Vector2 const& offset);

        /// Rotates this polygon around its center.
        /// @param angle The rotation angle in radians.
        void rotate(float32 angle);

        /// Scales this polygon around its center.
        /// @param scale The scale factors (width, height).
        void scale(Vector2 const& scale);

        /// Accesses a vertex by index (read-only).
        /// @param index The vertex index.
        /// @return The vertex at the specified index.
        Vector2 const& operator[](usize index) const;
        /// Accesses a vertex by index (mutable).
        /// @param index The vertex index.
        /// @return A reference to the vertex at the specified index.
        Vector2& operator[](usize index);
    };
}
