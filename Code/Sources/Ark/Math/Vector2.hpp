#pragma once

#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Types.hpp"

namespace Ark::Math
{
    /// Represents a 2D vector with x and y components.
    struct Vector2 final
    {
#pragma region Data

        union
        {
            struct
            {
                float32 x; ///< The x component of the vector.
                float32 y; ///< The y component of the vector.
            };

            float32 values[2]; ///< The array of values representing the vector.
        };

        /// A constant representing the zero vector.
        static const Vector2 Zero;

        /// A constant representing the unit vector with both components equal to 1.
        static const Vector2 One;

        /// A constant representing the unit vector along the x-axis.
        static const Vector2 UnitX;

        /// A constant representing the unit vector along the y-axis.
        static const Vector2 UnitY;

#pragma endregion

#pragma region Constructors

        Vector2() = default;

        /// Constructs a vector from individual x and y components.
        /// @param x The x component.
        /// @param y The y component.
        constexpr Vector2(float32 x, float32 y)
            : x{x}
            , y{y}
        {
        }

        /// Constructs a vector with both components set to the same value.
        /// @param value The value for both components.
        constexpr Vector2(float32 value)
            : x{value}
            , y{value}
        {
        }

        /// Constructs a vector from an array of two values.
        /// @param values Array of two float32 values in order [x, y].
        Vector2(float32 const values[2]);

        /// Constructs a vector from a Point2Float.
        /// @param point The source point.
        Vector2(Point2Float const& point);

#pragma endregion

#pragma region Accessors

        /// Checks if this vector is approximately zero.
        /// @return True if both components are near zero, false otherwise.
        bool isZero() const
        {
            return Math::isZero(x) && Math::isZero(y);
        }

        /// Returns the Euclidean length of this vector.
        /// @return The length of the vector.
        float32 getLength() const;

        /// Returns the squared Euclidean length of this vector.
        /// @return The squared length of the vector.
        float32 getLengthSquared() const;

        /// Returns the Euclidean distance between two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The distance between a and b.
        static float32 getDistance(Vector2 const& a, Vector2 const& b);

        /// Returns the squared Euclidean distance between two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The squared distance between a and b.
        static float32 getDistanceSquared(Vector2 const& a, Vector2 const& b);

        /// Calculates the angle between two vectors.
        static float32 getAngle(Vector2 const& a, Vector2 const& b);

#pragma endregion

#pragma region Operations

        /// Normalizes this vector in place.
        void normalize();

        /// Returns a normalized copy of this vector.
        /// @return A unit-length vector in the same direction.
        Vector2 getNormalized() const;

        /// Calculates the perpendicular of the vector, rotated 90 degrees counter-clockwise.
        Vector2 perpendicular() const;

        /// Computes the dot product of two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The scalar dot product.
        static float32 dot(Vector2 const& a, Vector2 const& b);

        /// Computes the dot product of this vector with another.
        /// @param other The other vector.
        /// @return The scalar dot product.
        float32 dot(Vector2 const& other);

        /// Computes the 2D cross product (scalar z-component of the 3D cross product).
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The scalar cross product.
        static float32 cross(Vector2 const& a, Vector2 const& b);

        /// Computes the 2D cross product with another vector.
        /// @param other The other vector.
        /// @return The scalar cross product.
        float32 cross(Vector2 const& other);

        /// Returns a copy of this vector translated by the given offset.
        /// @param translation The translation offset.
        /// @return The translated vector.
        Vector2 translated(Vector2 const& translation) const;

        /// Translates this vector in place by the given offset.
        /// @param translation The translation offset.
        void translate(Vector2 const& translation);

        /// Returns a copy of this vector rotated by the given angle.
        /// @param angle The angle in radians.
        /// @return The rotated vector.
        Vector2 rotated(float32 angle) const;

        /// Rotates this vector in place by the given angle.
        /// @param angle The angle in radians.
        void rotate(float32 angle);

        /// Returns a copy of this vector scaled by the given factors.
        /// @param scale Per-component scale factors.
        /// @return The scaled vector.
        Vector2 scaled(Vector2 const& scale) const;

        /// Scales this vector in place by the given factors.
        /// @param scale Per-component scale factors.
        void scale(Vector2 const& scale);

        /// Returns the component-wise minimum of two vectors.
        static Vector2 min(Vector2 const& a, Vector2 const& b);

        /// Returns the component-wise maximum of two vectors.
        static Vector2 max(Vector2 const& a, Vector2 const& b);

        /// Clamps each component of a vector between the corresponding min and max components.
        /// @param vector The vector to clamp.
        /// @param min The minimum bounds.
        /// @param max The maximum bounds.
        /// @return The clamped vector.
        static Vector2 clamp(Vector2 const& vector, Vector2 const& min, Vector2 const& max);

        /// Linearly interpolates between two vectors.
        /// @param start The starting vector (amount == 0).
        /// @param end The ending vector (amount == 1).
        /// @param amount The interpolation factor in [0, 1].
        /// @return The interpolated vector.
        static Vector2 lerp(Vector2 const& start, Vector2 const& end, float32 amount);

        /// Interpolates between two vectors using a smooth Hermite curve.
        /// @param start The starting vector (amount == 0).
        /// @param end The ending vector (amount == 1).
        /// @param amount The interpolation factor in [0, 1].
        /// @return The smooth-stepped vector.
        static Vector2 smoothStep(Vector2 const& start, Vector2 const& end, float32 amount);

        /// Returns the midpoint between two vectors.
        /// @param p1 The first vector.
        /// @param p2 The second vector.
        /// @return The midpoint vector.
        static Vector2 midpoint(Vector2 const& p1, Vector2 const& p2);

        /// Returns the unit normal to the edge from p1 to p2.
        /// @param p1 The start of the edge.
        /// @param p2 The end of the edge.
        /// @return The normalized perpendicular vector.
        static Vector2 normal(Vector2 const& p1, Vector2 const& p2);

        /// Transforms a vector by a 3x3 matrix.
        /// @param vector The vector to transform.
        /// @param transform The transformation matrix.
        /// @return The transformed vector.
        static Vector2 transform(Vector2 const& vector, Matrix3 const& transform);

        /// Projects a vector onto a direction defined by a normal.
        /// @param vector The vector to project.
        /// @param normal The projection direction (should be normalized).
        /// @return The projected vector.
        static Vector2 project(Vector2 const& vector, Vector2 const& normal);

        /// Reflects a vector about a surface normal.
        /// @param vector The incident vector.
        /// @param normal The surface normal (should be normalized).
        /// @return The reflected vector.
        static Vector2 reflect(Vector2 const& vector, Vector2 const& normal);

        /// Returns a copy of the vector rotated by an angle around the origin.
        /// @param vector The vector to rotate.
        /// @param angle The angle in radians.
        /// @return The rotated vector.
        static Vector2 rotate(Vector2 const& vector, float32 angle);

        /// Returns a copy of the vector moved towards the target by at most maxDistance.
        /// @param vector The starting vector.
        /// @param target The target vector.
        /// @param maxDistance The maximum movement distance.
        /// @return The moved vector.
        Vector2 moveTowards(Vector2 const& vector, Vector2 const& target, float32 maxDistance);

        /// Returns a vector with each component inverted (1/x, 1/y).
        /// @param vector The source vector.
        /// @return The inverted vector.
        static Vector2 invert(Vector2 const& vector);

        /// Returns a vector with each component negated.
        /// @param vector The source vector.
        /// @return The negated vector.
        static Vector2 negate(Vector2 const& vector);

        /// Returns the sum of two vectors.
        static Vector2 add(Vector2 const& a, Vector2 const& b);
        /// Returns the sum of a vector and a scalar.
        static Vector2 add(Vector2 const& vector, float32 scalar);

        /// Returns the difference of two vectors.
        static Vector2 subtract(Vector2 const& a, Vector2 const& b);
        /// Returns the difference of a vector and a scalar.
        static Vector2 subtract(Vector2 const& vector, float32 scalar);

        /// Returns the component-wise product of two vectors.
        static Vector2 multiply(Vector2 const& a, Vector2 const& b);
        /// Returns the product of a vector and a scalar.
        static Vector2 multiply(Vector2 const& vector, float32 scalar);

        /// Returns the component-wise quotient of two vectors.
        static Vector2 divide(Vector2 const& a, Vector2 const& b);
        /// Returns the quotient of a vector and a scalar.
        static Vector2 divide(Vector2 const& vector, float32 scalar);

#pragma endregion

#pragma region Operators

        float32& operator[](uint index)
        {
            return values[index];
        }

        float32 const& operator[](uint index) const
        {
            return values[index];
        }

        constexpr Vector2 operator-() const
        {
            return {-x, -y};
        }

        constexpr Vector2 operator+(Vector2 const& other) const
        {
            return {x + other.x, y + other.y};
        }

        constexpr Vector2 operator+(float32 scalar) const
        {
            return {x + scalar, y + scalar};
        }

        constexpr Vector2 operator-(Vector2 const& other) const
        {
            return {x - other.x, y - other.y};
        }

        constexpr Vector2 operator-(float32 scalar) const
        {
            return {x - scalar, y - scalar};
        }

        constexpr Vector2 operator*(Vector2 const& other) const
        {
            return {x * other.x, y * other.y};
        }

        constexpr Vector2 operator*(float32 scalar) const
        {
            return {x * scalar, y * scalar};
        }

        constexpr Vector2 operator/(Vector2 const& other) const
        {
            return {x / other.x, y / other.y};
        }

        constexpr Vector2 operator/(float32 scalar) const
        {
            return {x / scalar, y / scalar};
        }

        float32 operator|(Vector2 const& other) const
        {
            return dot(*this, other);
        }

        constexpr bool operator==(Vector2 const& other) const
        {
            return (x == other.x) && (y == other.y);
        }

        constexpr bool operator!=(Vector2 const& other) const
        {
            return (x != other.x) || (y != other.y);
        }

        constexpr Vector2& operator+=(Vector2 const& other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        constexpr Vector2& operator-=(Vector2 const& other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        constexpr Vector2& operator*=(Vector2 const& other)
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        constexpr Vector2& operator/=(Vector2 const& other)
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        constexpr Vector2& operator+=(float32 scalar)
        {
            x += scalar;
            y += scalar;
            return *this;
        }

        constexpr Vector2& operator-=(float32 scalar)
        {
            x -= scalar;
            y -= scalar;
            return *this;
        }

        constexpr Vector2& operator*=(float32 scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        constexpr Vector2& operator/=(float32 scalar)
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

#pragma endregion
    };
}
