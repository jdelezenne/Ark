#pragma once

#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Math
{
    /// A three-dimensional vector with x, y, and z components.
    /// Supports vector arithmetic, normalization, cross product, and interpolation operations.
    struct Vector3 final
    {
#pragma region Data

        union
        {
            struct
            {
                float32 x; ///< The x component of the vector.
                float32 y; ///< The y component of the vector.
                float32 z; ///< The z component of the vector.
            };

            float32 values[3]; ///< Array of values representing the vector components.
        };

        /// A constant representing the zero vector (0, 0, 0).
        static const Vector3 Zero;

        /// A constant representing the vector with all components equal to 1 (1, 1, 1).
        static const Vector3 One;

        /// A constant representing the unit vector along the x-axis (1, 0, 0).
        static const Vector3 UnitX;

        /// A constant representing the unit vector along the y-axis (0, 1, 0).
        static const Vector3 UnitY;

        /// A constant representing the unit vector along the z-axis (0, 0, 1).
        static const Vector3 UnitZ;

#pragma endregion

#pragma region Constructors

        Vector3() = default;

        /// Constructs a vector from individual x, y, and z components.
        /// @param x The x component.
        /// @param y The y component.
        /// @param z The z component.
        constexpr Vector3(float32 x, float32 y, float32 z)
            : x{x}
            , y{y}
            , z{z}
        {
        }

        /// Constructs a vector with all components set to the same value.
        /// @param value The value to set for all components.
        constexpr Vector3(float32 value)
            : x{value}
            , y{value}
            , z{value}
        {
        }

        /// Constructs a vector from an array of three values.
        /// @param values Array of three float32 values in order [x, y, z].
        Vector3(float32 const values[3])
        {
            Memory::copy(values, this->values, sizeof(this->values));
        }

#pragma endregion

#pragma region Accessors

        /// Checks if this vector is approximately zero.
        /// @return True if all components are near zero, false otherwise.
        bool isZero() const
        {
            return Math::isZero(x) && Math::isZero(y) && Math::isZero(z);
        }

        /// Returns the Euclidean length of this vector.
        /// @return The length of the vector.
        float32 getLength() const;

        /// Returns the squared Euclidean length of this vector.
        /// @return The squared length of the vector.
        float32 getLengthSquared() const;

#pragma endregion

#pragma region Operations

        /// Returns the Euclidean distance between two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The distance between a and b.
        static float32 getDistance(Vector3 const& a, Vector3 const& b);

        /// Returns the squared Euclidean distance between two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The squared distance between a and b.
        static float32 getDistanceSquared(Vector3 const& a, Vector3 const& b);

        /// Returns a normalized copy of the given vector.
        /// @param v The vector to normalize.
        /// @return A unit-length vector in the same direction as v.
        static Vector3 normalized(Vector3 const& v)
        {
            return v.getNormalized();
        }

        /// Normalizes this vector in place.
        void normalize();

        /// Returns a normalized copy of this vector.
        /// @return A unit-length vector in the same direction as this vector.
        Vector3 getNormalized() const;

        static float32 dot(Vector3 const& a, Vector3 const& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        float32 dot(Vector3 const& other) const
        {
            return dot(*this, other);
        }

        static Vector3 cross(Vector3 const& a, Vector3 const& b)
        {
            return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
        }

        /// Computes the cross product of this vector with another.
        /// @param other The other vector.
        /// @return A vector perpendicular to both this vector and other.
        Vector3 cross(Vector3 const& other) const
        {
            return cross(*this, other);
        }

        /// Computes the angle between two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The angle in radians between a and b.
        float32 getAngle(Vector3 const& a, Vector3 const& b);

        static Vector3 lerp(Vector3 const& start, Vector3 const& end, float32 amount)
        {
            return {
                Math::lerp(start.x, end.x, amount),
                Math::lerp(start.y, end.y, amount),
                Math::lerp(start.z, end.z, amount),
            };
        }

        /// Interpolates between two vectors using a smooth Hermite curve.
        /// @param start The starting vector (amount == 0).
        /// @param end The ending vector (amount == 1).
        /// @param amount The interpolation factor in [0, 1].
        /// @return The smooth-stepped interpolated vector.
        static Vector3 smoothStep(Vector3 const& start, Vector3 const& end, float32 amount)
        {
            amount = Math::smoothStep(amount);
            return {
                Math::lerp(start.x, end.x, amount),
                Math::lerp(start.y, end.y, amount),
                Math::lerp(start.z, end.z, amount),
            };
        }

        /// Transforms a vector by a 3x3 matrix.
        /// @param vector The vector to transform.
        /// @param transform The transformation matrix.
        /// @return The transformed vector.
        static Vector3 transform(Vector3 const& vector, Matrix3 const& transform);

        /// Projects a vector onto a plane defined by a normal.
        /// @param vector The vector to project.
        /// @param normal The plane normal (should be normalized).
        /// @return The projected vector.
        static Vector3 project(Vector3 const& vector, Vector3 const& normal);

        /// Computes the refraction vector for an incident ray and surface normal.
        /// @param incident The incident vector (should be normalized).
        /// @param normal The surface normal (should be normalized).
        /// @param ratio The ratio of indices of refraction.
        /// @return The refracted vector.
        static Vector3 refract(Vector3 const& incident, Vector3 const& normal, float32 ratio);

        /// Computes the barycenter coordinates for a <point> with respect to triangle (a, b, c).
        /// @remarks Assumes the point is on the plane of the triangle.
        static Vector3 getBarycenter(Vector3 const& point, Vector3 const& a, Vector3 const& b, Vector3 const& c);

#pragma endregion

#pragma region Operators

        /// Returns a reference to the component at the given index.
        /// @param index Component index (0=x, 1=y, 2=z).
        float32& operator[](uint index)
        {
            return values[index];
        }

        /// Returns a const reference to the component at the given index.
        /// @param index Component index (0=x, 1=y, 2=z).
        float32 const& operator[](uint index) const
        {
            return values[index];
        }

        /// Returns the negation of this vector.
        Vector3 operator-() const
        {
            return {-x, -y, -z};
        }

        Vector3 operator+(Vector3 const& other) const
        {
            return {x + other.x, y + other.y, z + other.z};
        }

        Vector3 operator+(float32 scalar) const
        {
            return {x + scalar, y + scalar, z + scalar};
        }

        Vector3 operator-(Vector3 const& other) const
        {
            return {x - other.x, y - other.y, z - other.z};
        }

        Vector3 operator-(float32 scalar) const
        {
            return {x - scalar, y - scalar, z - scalar};
        }

        Vector3 operator*(Vector3 const& other) const
        {
            return {x * other.x, y * other.y, z * other.z};
        }

        Vector3 operator*(float32 scalar) const
        {
            return {x * scalar, y * scalar, z * scalar};
        }

        Vector3 operator/(Vector3 const& other) const
        {
            return {x / other.x, y / other.y, z / other.z};
        }

        Vector3 operator/(float32 scalar) const
        {
            return {x / scalar, y / scalar, z / scalar};
        }

        /// Computes the cross product (operator alias for cross()).
        Vector3 operator^(Vector3 const& other) const
        {
            return cross(*this, other);
        }

        /// Computes the dot product (operator alias for dot()).
        float32 operator|(Vector3 const& other) const
        {
            return dot(*this, other);
        }

        bool operator==(Vector3 const& other) const
        {
            return (x == other.x) && (y == other.y) && (z == other.z);
        }

        bool operator!=(Vector3 const& other) const
        {
            return (x != other.x) || (y != other.y) || (z != other.z);
        }

        Vector3& operator+=(Vector3 const& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vector3& operator-=(Vector3 const& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vector3& operator*=(Vector3 const& other)
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        Vector3& operator/=(Vector3 const& other)
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        Vector3& operator+=(float32 scalar)
        {
            x += scalar;
            y += scalar;
            z += scalar;
            return *this;
        }

        Vector3& operator-=(float32 scalar)
        {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            return *this;
        }

        Vector3& operator*=(float32 scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vector3& operator/=(float32 scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

#pragma endregion
    };
}
