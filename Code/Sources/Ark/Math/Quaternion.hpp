#pragma once

#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Math
{
    /// A structure that represents a quaternion, which is used to represent rotations in 3D space.
    struct Quaternion
    {
        union
        {
            struct
            {
                float32 x; ///< The x-component of the quaternion.
                float32 y; ///< The y-component of the quaternion.
                float32 z; ///< The z-component of the quaternion.
                float32 w; ///< The w-component of the quaternion, often representing the scalar part.
            };

            float32 values[4]; ///< An array to access the quaternion components as a single array.
        };

        /// A constant representing a zero quaternion.
        static Quaternion const Zero;

        /// A constant representing an identity quaternion, which represents no rotation.
        static Quaternion const Identity;

        constexpr Quaternion() = default;

        /// Constructs a quaternion from individual components.
        /// @param x The x-component.
        /// @param y The y-component.
        /// @param z The z-component.
        /// @param w The w-component (scalar part).
        constexpr Quaternion(float32 x, float32 y, float32 z, float32 w)
            : x{x}
            , y{y}
            , z{z}
            , w{w}
        {
        }

        /// Constructs a quaternion with all components set to the same value.
        /// @param value The value to assign to all components.
        constexpr Quaternion(float32 value)
            : x{value}
            , y{value}
            , z{value}
            , w{value}
        {
        }

        /// Constructs a quaternion from an array of four values.
        /// @param values Array of four float32 values in order [x, y, z, w].
        Quaternion(float32 const values[4])
        {
            Memory::copy(values, this->values, 4 * sizeof(float32));
        }

        /// Checks if this quaternion is approximately zero.
        /// @return True if all components are near zero, false otherwise.
        bool isZero() const
        {
            return Math::isZero(x) && Math::isZero(y) && Math::isZero(z) && Math::isZero(w);
        }

        /// Returns the length (magnitude) of this quaternion.
        /// @return The length of the quaternion.
        float32 getLength();

        /// Returns the squared length of this quaternion.
        /// @return The squared length of the quaternion.
        float32 getLengthSquared();

        /// Normalizes this quaternion in place.
        void normalize();

        /// Returns a normalized copy of this quaternion.
        /// @return A unit-length quaternion.
        Quaternion getNormalized() const;

        /// Inverts this quaternion in place.
        void invert();

        /// Returns the inverse of this quaternion.
        /// @return The inverse quaternion.
        Quaternion getInversed() const;

        /// Conjugates this quaternion in place (negates the vector part).
        void conjugate();

        /// Returns the conjugate of this quaternion.
        /// @return The conjugate quaternion.
        Quaternion getConjugate() const;

        /// Computes the dot product of two quaternions.
        /// @param a The first quaternion.
        /// @param b The second quaternion.
        /// @return The scalar dot product.
        static float32 dot(Quaternion const& a, Quaternion const& b);

        /// Computes the dot product of this quaternion with another.
        /// @param other The other quaternion.
        /// @return The scalar dot product.
        float32 dot(Quaternion const& other) const;

        /// Returns the angle between two quaternions in radians.
        /// @param a The first quaternion.
        /// @param b The second quaternion.
        /// @return The angle in radians.
        static float32 angle(Quaternion const& a, Quaternion const& b);

        /// Creates a quaternion from an axis-angle representation.
        /// @param axis The axis of rotation (should be normalized).
        /// @param angle The angle of rotation in radians.
        /// @return The corresponding quaternion.
        static Quaternion fromAxisAngle(Vector3 const& axis, float32 angle);

        /// Creates a quaternion from Euler angles in radians.
        /// @param pitch The rotation around the x-axis in radians.
        /// @param yaw The rotation around the y-axis in radians.
        /// @param roll The rotation around the z-axis in radians.
        /// @return The corresponding quaternion.
        static Quaternion fromEulerAngles(float32 pitch, float32 yaw, float32 roll);

        /// Creates a quaternion that rotates from the default forward direction to face the given forward vector.
        /// @param forward The desired forward direction (should be normalized).
        /// @param up The desired up direction (should be normalized).
        /// @return The look-rotation quaternion.
        static Quaternion lookRotation(Vector3 const& forward, Vector3 const& up);

        /// Creates a quaternion from a rotation matrix.
        /// @remarks Based on http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
        /// @param m The rotation matrix.
        /// @return The corresponding quaternion.
        static Quaternion fromMatrix(Matrix4 const& m);

        /// Converts this quaternion to a 4x4 rotation matrix.
        /// @return The rotation matrix.
        Matrix4 toMatrix() const;

        /// Transforms a vector by a quaternion rotation.
        /// @param v The vector to transform.
        /// @param q The rotation quaternion.
        /// @return The transformed vector.
        static Vector3 transformVector(Vector3 const& v, Quaternion const& q);

        /// Transforms a vector by this quaternion rotation.
        /// @param v The vector to transform.
        /// @return The transformed vector.
        Vector3 transformVector(Vector3 const& v) const;

        /// Linearly interpolates between two quaternions.
        /// @param start The starting quaternion (amount == 0).
        /// @param end The ending quaternion (amount == 1).
        /// @param amount The interpolation factor in [0, 1].
        /// @return The interpolated quaternion.
        static Quaternion lerp(Quaternion const& start, Quaternion const& end, float32 amount);

        /// Spherically interpolates between two quaternions.
        /// @param start The starting quaternion (amount == 0).
        /// @param end The ending quaternion (amount == 1).
        /// @param amount The interpolation factor in [0, 1].
        /// @return The slerp-interpolated quaternion.
        static Quaternion slerp(Quaternion const& start, Quaternion const& end, float32 amount);

        /// Interpolates between two quaternions using a smooth Hermite curve.
        /// @param start The starting quaternion (amount == 0).
        /// @param end The ending quaternion (amount == 1).
        /// @param amount The interpolation factor in [0, 1].
        /// @return The smooth-stepped interpolated quaternion.
        static Quaternion smoothStep(Quaternion const& start, Quaternion const& end, float32 amount);

        /// Multiplies two quaternions.
        /// @param a The left-hand quaternion.
        /// @param b The right-hand quaternion.
        /// @return The quaternion product.
        static Quaternion multiply(Quaternion const& a, Quaternion const& b);

        Quaternion operator*(Quaternion const& other) const
        {
            return multiply(*this, other);
        }

        Quaternion operator+(Quaternion const& other) const
        {
            return {x + other.x, y + other.y, z + other.z, w + other.w};
        }

        Quaternion operator-(Quaternion const& other) const
        {
            return {x - other.x, y - other.y, z - other.z, w - other.w};
        }

        Quaternion operator*(float32 scalar) const
        {
            return {x * scalar, y * scalar, z * scalar, w * scalar};
        }

        Quaternion operator/(Quaternion const& other) const
        {
            return multiply(*this, other.getInversed());
        }

        Quaternion operator/(float32 scalar) const
        {
            return {x / scalar, y / scalar, z / scalar, w / scalar};
        }

        bool operator==(Quaternion const& other) const
        {
            return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
        }

        bool operator!=(Quaternion const& other) const
        {
            return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
        }

        Quaternion& operator*=(Quaternion const& other)
        {
            *this = multiply(*this, other);
            return *this;
        }

        Quaternion& operator/=(Quaternion const& other)
        {
            *this = *this / other;
            return *this;
        }

        Quaternion& operator*=(float32 scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        Quaternion& operator/=(float32 scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }
    };
}
