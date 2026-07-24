#pragma once

#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Math
{
    /// A four-dimensional vector with x, y, z, and w components.
    /// Supports vector arithmetic, normalization, and interpolation operations.
    struct Vector4 final
    {
        union
        {
            struct
            {
                float32 x;
                float32 y;
                float32 z;
                float32 w;
            };

            float32 values[4];
        };

        static const Vector4 Zero;

        static const Vector4 One;

        static const Vector4 UnitX;

        static const Vector4 UnitY;

        static const Vector4 UnitZ;

        static const Vector4 UnitW;

        Vector4() = default;

        /// Constructs a vector from individual x, y, z, w components.
        /// @param x The x component.
        /// @param y The y component.
        /// @param z The z component.
        /// @param w The w component.
        constexpr Vector4(float32 x, float32 y, float32 z, float32 w)
            : x{x}
            , y{y}
            , z{z}
            , w{w}
        {
        }

        /// Constructs a vector with all components set to the same value.
        /// @param value The value to set for all components.
        constexpr Vector4(float32 value)
            : x{value}
            , y{value}
            , z{value}
            , w{value}
        {
        }

        /// Constructs a vector from an array of four values.
        /// @param values Array of four float32 values in order [x, y, z, w].
        Vector4(float32 const values[4])
        {
            Memory::copy(values, this->values, sizeof(this->values));
        }

        /// Checks if this vector is approximately zero.
        /// @return True if all components are near zero, false otherwise.
        bool isZero() const
        {
            return Math::isZero(x) && Math::isZero(y) && Math::isZero(z) && Math::isZero(w);
        }

        /// Returns the Euclidean length of this vector.
        /// @return The length of the vector.
        float32 getLength();

        /// Computes the Euclidean distance between two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The distance between a and b.
        static float32 distance(Vector4 const& a, Vector4 const& b);

        /// Computes the squared Euclidean distance between two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The squared distance between a and b.
        static float32 distanceSquared(Vector4 const& a, Vector4 const& b);

        /// Normalizes this vector to unit length.
        void normalize();

        /// Returns a normalized copy of this vector.
        /// @return A vector with the same direction but unit length.
        Vector4 getNormalized() const;

        /// Computes the dot product of two vectors.
        /// @param a The first vector.
        /// @param b The second vector.
        /// @return The dot product of a and b.
        static float32 dot(Vector4 const& a, Vector4 const& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        /// Computes the dot product of this vector with another.
        /// @param other The other vector.
        /// @return The dot product.
        float32 dot(Vector4 const& other) const
        {
            return dot(*this, other);
        }

        /// Linearly interpolates between two vectors.
        /// @param start The start vector.
        /// @param end The end vector.
        /// @param amount The interpolation amount, typically between 0 and 1.
        /// @return The interpolated vector.
        static Vector4 lerp(Vector4 const& start, Vector4 const& end, float32 amount)
        {
            return {
                Math::lerp(start.x, end.x, amount),
                Math::lerp(start.y, end.y, amount),
                Math::lerp(start.z, end.z, amount),
                Math::lerp(start.w, end.w, amount),
            };
        }

        /// Smoothly interpolates between two vectors with easing.
        /// @param start The start vector.
        /// @param end The end vector.
        /// @param amount The interpolation amount, typically between 0 and 1.
        /// @return The smoothly interpolated vector.
        static Vector4 smoothStep(Vector4 const& start, Vector4 const& end, float32 amount)
        {
            amount = Math::smoothStep(amount);
            return {
                Math::lerp(start.x, end.x, amount),
                Math::lerp(start.y, end.y, amount),
                Math::lerp(start.z, end.z, amount),
                Math::lerp(start.w, end.w, amount),
            };
        }

        Vector4 operator-() const
        {
            return {-x, -y, -z, -w};
        }

        Vector4 operator+(Vector4 const& other) const
        {
            return {x + other.x, y + other.y, z + other.z, w + other.w};
        }

        Vector4 operator+(float32 scalar) const
        {
            return {x + scalar, y + scalar, z + scalar, w + scalar};
        }

        Vector4 operator-(Vector4 const& other) const
        {
            return {x - other.x, y - other.y, z - other.z, w - other.w};
        }

        Vector4 operator-(float32 scalar) const
        {
            return {x - scalar, y - scalar, z - scalar, w - scalar};
        }

        Vector4 operator*(Vector4 const& other) const
        {
            return {x * other.x, y * other.y, z * other.z, w * other.w};
        }

        Vector4 operator*(float32 scalar) const
        {
            return {x * scalar, y * scalar, z * scalar, w * scalar};
        }

        Vector4 operator/(Vector4 const& other) const
        {
            return {x / other.x, y / other.y, z / other.z, w / other.w};
        }

        Vector4 operator/(float32 scalar) const
        {
            return {x / scalar, y / scalar, z / scalar, w / scalar};
        }

        float32 operator|(Vector4 const& other) const
        {
            return dot(*this, other);
        }

        bool operator==(Vector4 const& other) const
        {
            return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
        }

        bool operator!=(Vector4 const& other) const
        {
            return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
        }

        Vector4& operator+=(Vector4 const& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        Vector4& operator-=(Vector4 const& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        Vector4& operator*=(Vector4 const& other)
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }

        Vector4& operator/=(Vector4 const& other)
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return *this;
        }

        Vector4& operator+=(float32 scalar)
        {
            x += scalar;
            y += scalar;
            z += scalar;
            w += scalar;
            return *this;
        }

        Vector4& operator-=(float32 scalar)
        {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            w -= scalar;
            return *this;
        }

        Vector4& operator*=(float32 scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        Vector4& operator/=(float32 scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }
    };
}
