#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Math/Types.hpp"

namespace Ark
{
    /// A structure that contains a point in a two-dimensional coordinate system.
    template <Concepts::Arithmetic T>
    struct Point2 final
    {
        /// Gets a special value that represents a zero point.
        static const Point2 Zero;

        union
        {
            struct
            {
                T x; ///< The x-coordinate of the point.
                T y; ///< The y-coordinate of the point.
            };

            T coordinates[2]; ///< Array of coordinates for indexed access.
        };

        inline Point2()
            : x{T(0)}
            , y{T(0)}
        {
        }

        /// Constructs a point with the given x and y coordinates.
        /// @param x The x-coordinate.
        /// @param y The y-coordinate.
        inline Point2(T x, T y)
            : x{x}
            , y{y}
        {
        }

        template <typename U>
        constexpr bool isZero(U const& value)
        requires Concepts::Integral<U>
        {
            return value == U{0};
        }

        template <typename U>
        constexpr bool isZero(U const& value)
        requires Concepts::FloatingPoint<U>
        {
            return isNearZero(value);
        }

        /// Converts this point to a floating-point Vector2.
        /// @return A Vector2 with the point's coordinates as components.
        Math::Vector2 toVector2() const;

        inline T& operator[](int index)
        {
            return coordinates[index];
        }

        inline T const& operator[](int index) const
        {
            return coordinates[index];
        }

        inline bool operator==(Point2 const& point) const
        {
            return (x == point.x && y == point.y);
        }

        inline bool operator!=(Point2 const& point) const
        {
            return (x != point.x || y != point.y);
        }

        Point2<T> operator-() const
        {
            return {
                -x,
                -y,
            };
        }

        Point2<T> operator+(Point2<T> const& point) const
        {
            return {
                x + point.x,
                y + point.y,
            };
        }

        void operator+=(Point2<T> const& point)
        {
            x += point.x;
            y += point.y;
        }

        Point2<T> operator-(Point2<T> const& point) const
        {
            return {
                x - point.x,
                y - point.y,
            };
        }

        void operator-=(Point2<T> const& point)
        {
            x -= point.x;
            y -= point.y;
        }

        Point2<T> operator*(Point2<T> const& point) const
        {
            return {
                x * point.x,
                y * point.y,
            };
        }

        void operator*=(Point2<T> const& point)
        {
            x *= point.x;
            y *= point.y;
        }

        template <Concepts::Arithmetic U>
        Point2<T> operator/(Point2<U> const& point) const
        {
            return {
                x / point.x,
                y / point.y,
            };
        }

        template <Concepts::Arithmetic U>
        void operator/=(Point2<U> const& point)
        {
            x /= point.x;
            y /= point.y;
        }

        template <Concepts::Arithmetic U>
        Point2<T> operator+(U scalar) const
        {
            return {
                x + scalar,
                y + scalar,
            };
        }

        template <Concepts::Arithmetic U>
        void operator+=(U scalar)
        {
            x += scalar;
            y += scalar;
        }

        template <Concepts::Arithmetic U>
        Point2<T> operator-(U scalar) const
        {
            return {
                x - scalar,
                y - scalar,
            };
        }

        void operator-=(T scalar)
        {
            x -= scalar;
            y -= scalar;
        }

        Point2<T> operator*(T scalar) const
        {
            return {
                x * scalar,
                y * scalar,
            };
        }

        void operator*=(T scalar)
        {
            x *= scalar;
            y *= scalar;
        }

        Point2<T> operator/(T scalar) const
        {
            return {
                x / scalar,
                y / scalar,
            };
        }

        void operator/=(T scalar)
        {
            x /= scalar;
            y /= scalar;
        }
    };

    template <Concepts::Arithmetic T>
    const Point2<T> Point2<T>::Zero = {T(0), T(0)};

    using Point2Int = Point2<int32>;
    using Point2UInt = Point2<uint32>;
    using Point2Float = Point2<float32>;
}
