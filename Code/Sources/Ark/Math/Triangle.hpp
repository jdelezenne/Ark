#pragma once

#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector2.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    /// A 2D triangle defined by three vertices.
    /// Supports area and perimeter calculations.
    struct Triangle2 final
    {
    public:
        /// The first vertex.
        Vector2 p0;
        /// The second vertex.
        Vector2 p1;
        /// The third vertex.
        Vector2 p2;

    public:
        /// Constructs a triangle with default vertices (all zero).
        constexpr Triangle2()
        {
        }

        /// Constructs a triangle with the specified vertices.
        /// @param p0 The first vertex.
        /// @param p1 The second vertex.
        /// @param p2 The third vertex.
        constexpr Triangle2(Vector2 const& p0, Vector2 const& p1, Vector2 const& p2)
            : p0{p0}
            , p1{p1}
            , p2{p2}
        {
        }

        /// Computes the area of this triangle.
        /// @return The area of the triangle.
        float32 getArea() const
        {
            return Math::abs((p1 - p0).cross(p2 - p0)) / 2;
        }

        /// Computes the perimeter of this triangle.
        /// @return The sum of the lengths of all three sides.
        float32 getPerimeter() const
        {
            return (p1 - p0).getLength() +
                   (p2 - p0).getLength() +
                   (p2 - p1).getLength();
        }
    };

    /// A 3D triangle defined by three vertices.
    /// Used for geometric queries and mesh representation.
    struct Triangle3 final
    {
    public:
        /// The first vertex.
        Vector3 p0;
        /// The second vertex.
        Vector3 p1;
        /// The third vertex.
        Vector3 p2;

    public:
        /// Constructs a triangle with default vertices (all zero).
        constexpr Triangle3()
        {
        }

        /// Constructs a triangle with the specified vertices.
        /// @param p0 The first vertex.
        /// @param p1 The second vertex.
        /// @param p2 The third vertex.
        constexpr Triangle3(Vector3 const& p0, Vector3 const& p1, Vector3 const& p2)
            : p0{p0}
            , p1{p1}
            , p2{p2}
        {
        }
    };
}
