#pragma once

#include "Ark/Core/Traits.hpp"
#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Point2.hpp"
#include "Ark/Math/Size2.hpp"

namespace Ark
{
    /// A structure that contains the location and dimensions of a rectangle.
    template <Concepts::Arithmetic T>
    struct Rect final
    {
        union
        {
            struct
            {
                Point2<T> origin; ///< A point that specifies the coordinates of the rectangle’s origin.
                Size2<T> size;    ///< A size that specifies the height and width of the rectangle.
            };

            struct
            {
                T x;      ///< The x-coordinate of the rectangle’s origin.
                T y;      ///< The y-coordinate of the rectangle’s origin.
                T width;  ///< The width of the rectangle.
                T height; ///< The height of the rectangle.
            };
        };

        /// Gets a special value that represents an empty rectangle.
        static Rect const Empty;

        constexpr Rect()
            : x{T(0)}
            , y{T(0)}
            , width{T(0)}
            , height{T(0)}
        {
        }

        /// Constructs a rectangle from an origin point and a size.
        /// @param origin The origin (top-left) of the rectangle.
        /// @param size The dimensions of the rectangle.
        constexpr Rect(Point2<T> const& origin, Size2<T> const& size)
            : origin{origin}
            , size{size}
        {
        }

        /// Constructs a rectangle from individual x, y, width, and height values.
        /// @param x The x-coordinate of the origin.
        /// @param y The y-coordinate of the origin.
        /// @param width The width of the rectangle.
        /// @param height The height of the rectangle.
        constexpr Rect(T x, T y, T width, T height)
            : x{x}
            , y{y}
            , width{width}
            , height{height}
        {
        }

        /// Returns the epsilon value used for boundary comparisons.
        /// For floating-point types returns 0; for integral types returns 1.
        static constexpr auto epsilon()
        {
            if constexpr (Traits::isFloatingPoint<T>)
            {
                return static_cast<T>(0);
            }
            else
            {
                return static_cast<T>(1);
            }
        }

        /// Gets the x-axis value of the left of the rectangle.
        constexpr T getLeft() const
        {
            return x;
        }

        /// Gets the y-axis value of the top of the rectangle.
        constexpr T getTop() const
        {
            return y;
        }

        /// Gets the y-axis value of the right of the rectangle.
        constexpr T getRight() const
        {
            return x + width;
        }

        /// Gets the y-axis value of the bottom of the rectangle.
        constexpr T getBottom() const
        {
            return y + height;
        }

        /// Gets a value that indicates whether the rectangle is empty.
        constexpr bool isEmpty() const
        {
            return *this == Empty;
        }

        /// Gets a value that indicates whether the size is valid (non-negative dimensions).
        /// @return True if width and height are both non-negative.
        constexpr bool isValid() const
        {
            return size.isValid();
        }

        /// Returns the center point of the rectangle.
        /// @return The center point.
        constexpr Point2<T> getCenter() const
        {
            return {x + width / T(2.0), y + height / T(2.0)};
        }

        /// Computes whether two rectangles are approximately equal within an epsilon.
        /// @param a The first rectangle.
        /// @param b The second rectangle.
        /// @param eps The tolerance.
        /// @return True if all components differ by at most eps.
        static constexpr bool equalsEpsilon(Rect const& a, Rect const& b, T eps)
        {
            if constexpr (Traits::isFloatingPoint<T>)
            {
                return (Math::abs(a.x - b.x) <= eps &&
                        Math::abs(a.y - b.y) <= eps &&
                        Math::abs(a.width - b.width) <= eps &&
                        Math::abs(a.height - b.height) <= eps);
            }
            else
            {
                return (a == b);
            }
        }

        /// Creates a rectangle that is the bounding rect of two points.
        /// @param point1 The first point.
        /// @param point2 The second point.
        /// @return The bounding rectangle.
        static Rect fromPoints(Point2<T> const& point1, Point2<T> const& point2)
        {
            T left = Math::min(point1.x, point2.x);
            T right = Math::max(point1.x, point2.x);
            T top = Math::min(point1.y, point2.y);
            T bottom = Math::max(point1.y, point2.y);

            return Rect<T>(left, top, right, bottom);
        }

        /// Creates a rectangle from left, top, right, and bottom edge coordinates.
        /// @param left The left edge x-coordinate.
        /// @param top The top edge y-coordinate.
        /// @param right The right edge x-coordinate.
        /// @param bottom The bottom edge y-coordinate.
        /// @return The constructed rectangle.
        static constexpr Rect fromCorners(T left, T top, T right, T bottom)
        {
            return {
                left,
                top,
                right - left,
                bottom - left,
            };
        }

        /// Expands or shrinks the rectangle by using the specified width and height amounts.
        void inflate(T width, T height)
        {
            ART_ensure(!isEmpty());

            x -= width;
            y -= height;

            width += width * T(2.0);
            height += height * T(2.0);

            if (width < T(0.0) || height < T(0.0))
            {
                *this = Empty;
            }
        }

        /// Indicates whether the rectangle contains the specified point.
        bool contains(Point2<T> const& point)
        {
            return (point.x >= x) &&
                   (point.x - width <= x) &&
                   (point.y >= y) &&
                   (point.y - height <= y);
        }

        /// Finds the intersection of the rectangle and the specified rectangle.
        Rect intersectWith(Rect const& rect)
        {
            T left = Math::max(getLeft(), rect.getLeft());
            T right = Math::min(getRight(), rect.getRight());
            T top = Math::max(getTop(), rect.getTop());
            T bottom = Math::min(getBottom(), rect.getBottom());

            if (right >= left && bottom >= top)
            {
                return Rect<T>(left, top, right - left, bottom - top);
            }
            else
            {
                return Empty;
            }
        }

        /// Finds the union of the rectangle and the specified rectangle.
        Rect unionWith(Rect const& rect)
        {
            T left = Math::min(getLeft(), rect.getLeft());
            T right = Math::max(getRight(), rect.getRight());
            T top = Math::min(getTop(), rect.getTop());
            T bottom = Math::max(getBottom(), rect.getBottom());

            return Rect<T>(left, top, right - left, bottom - top);
        }

        /// Checks whether two rectangles intersect.
        /// @param a The first rectangle.
        /// @param b The second rectangle.
        /// @return True if the rectangles overlap, false otherwise.
        static bool hasIntersection(Rect const& a, Rect const& b)
        {
            if (a.isEmpty() || b.isEmpty())
            {
                return false;
            }

            T amin = a.x;
            T amax = a.x + a.width;
            T bmin = b.x;
            T bmax = b.x + b.width;

            if (bmin > amin)
            {
                amin = bmin;
            }

            if (bmax < amax)
            {
                amax = bmax;
            }

            if ((amax - epsilon()) < amin)
            {
                return false;
            }

            amin = a.y;
            amax = a.y + a.height;
            bmin = b.y;
            bmax = b.y + b.height;

            if (bmin > amin)
            {
                amin = bmin;
            }

            if (bmax < amax)
            {
                amax = bmax;
            }

            if ((amax - epsilon()) < amin)
            {
                return false;
            }

            return true;
        }

        /// Calculates the smallest rectangle enclosing a set of points.
        /// @param points Pointer to the array of points.
        /// @param count The number of points.
        /// @param clip Optional clipping rectangle; only points inside it are considered.
        /// @param result Output rectangle; only written when return value is true.
        /// @return True if at least one point was enclosed, false otherwise.
        static bool enclosePoints(Point2<T> const* points, int count, Rect const* clip, Rect* result)
        {
            if (points == nullptr || count < 1)
            {
                return false;
            }

            T minx = static_cast<T>(0);
            T miny = static_cast<T>(0);
            T maxx = static_cast<T>(0);
            T maxy = static_cast<T>(0);

            if (clip != nullptr)
            {
                bool added = false;
                const T clip_minx = clip->x;
                const T clip_miny = clip->y;
                const T clip_maxx = clip->x + clip->width - epsilon();
                const T clip_maxy = clip->y + clip->height - epsilon();

                if (clip->isEmpty())
                {
                    return false;
                }

                for (int i = 0; i < count; ++i)
                {
                    const T x = points[i].x;
                    const T y = points[i].y;

                    if (x < clip_minx || x > clip_maxx || y < clip_miny || y > clip_maxy)
                    {
                        continue;
                    }

                    if (!added)
                    {
                        if (result == nullptr)
                        {
                            return true;
                        }

                        minx = maxx = x;
                        miny = maxy = y;
                        added = true;
                        continue;
                    }

                    if (x < minx)
                    {
                        minx = x;
                    }
                    else if (x > maxx)
                    {
                        maxx = x;
                    }

                    if (y < miny)
                    {
                        miny = y;
                    }
                    else if (y > maxy)
                    {
                        maxy = y;
                    }
                }

                if (!added)
                {
                    return false;
                }
            }
            else
            {
                if (result == nullptr)
                {
                    return true;
                }

                minx = maxx = points[0].x;
                miny = maxy = points[0].y;

                for (int i = 1; i < count; ++i)
                {
                    const T x = points[i].x;
                    const T y = points[i].y;

                    if (x < minx)
                    {
                        minx = x;
                    }
                    else if (x > maxx)
                    {
                        maxx = x;
                    }

                    if (y < miny)
                    {
                        miny = y;
                    }
                    else if (y > maxy)
                    {
                        maxy = y;
                    }
                }
            }

            if (result)
            {
                result->x = minx;
                result->y = miny;
                result->width = (maxx - minx) + epsilon();
                result->height = (maxy - miny) + epsilon();
            }

            return true;
        }

        /// Clips a line segment to the rectangle using Cohen-Sutherland clipping.
        /// @param r The clipping rectangle.
        /// @param x1 The x-coordinate of the line start; updated on success.
        /// @param y1 The y-coordinate of the line start; updated on success.
        /// @param x2 The x-coordinate of the line end; updated on success.
        /// @param y2 The y-coordinate of the line end; updated on success.
        /// @return True if any part of the line is inside the rectangle.
        static bool intersectRectAndLine(Rect const& r, T& x1, T& y1, T& x2, T& y2)
        {
            if (r.isEmpty())
            {
                return false;
            }

            const T rectx1 = r.x;
            const T recty1 = r.y;
            const T rectx2 = r.x + r.width - epsilon();
            const T recty2 = r.y + r.height - epsilon();

            T _x1 = x1;
            T _y1 = y1;
            T _x2 = x2;
            T _y2 = y2;

            auto computeOutCode = [&](T x, T y) -> int
            {
                int code = 0;

                if (y < recty1)
                {
                    code |= 2;
                }
                else if (y > recty2)
                {
                    code |= 1;
                }

                if (x < rectx1)
                {
                    code |= 4;
                }
                else if (x > rectx2)
                {
                    code |= 8;
                }

                return code;
            };

            if (_x1 >= rectx1 && _x1 <= rectx2 && _x2 >= rectx1 && _x2 <= rectx2 &&
                _y1 >= recty1 && _y1 <= recty2 && _y2 >= recty1 && _y2 <= recty2)
            {
                return true;
            }

            if ((_x1 < rectx1 && _x2 < rectx1) || (_x1 > rectx2 && _x2 > rectx2) ||
                (_y1 < recty1 && _y2 < recty1) || (_y1 > recty2 && _y2 > recty2))
            {
                return false;
            }

            if (_y1 == _y2)
            {
                if (_x1 < rectx1)
                {
                    x1 = rectx1;
                }
                else if (_x1 > rectx2)
                {
                    x1 = rectx2;
                }

                if (_x2 < rectx1)
                {
                    x2 = rectx1;
                }
                else if (_x2 > rectx2)
                {
                    x2 = rectx2;
                }

                return true;
            }

            if (_x1 == _x2)
            {
                if (_y1 < recty1)
                {
                    y1 = recty1;
                }
                else if (_y1 > recty2)
                {
                    y1 = recty2;
                }

                if (_y2 < recty1)
                {
                    y2 = recty1;
                }
                else if (_y2 > recty2)
                {
                    y2 = recty2;
                }

                return true;
            }

            int outcode1 = computeOutCode(_x1, _y1);
            int outcode2 = computeOutCode(_x2, _y2);

            while (outcode1 || outcode2)
            {
                if (outcode1 & outcode2)
                {
                    return false;
                }

                T x = static_cast<T>(0);
                T y = static_cast<T>(0);

                if (outcode1)
                {
                    if (outcode1 & 2)
                    {
                        y = recty1;
                        x = static_cast<T>(_x1 + (static_cast<long double>(_x2 - _x1) * (y - _y1)) / (_y2 - _y1));
                    }
                    else if (outcode1 & 1)
                    {
                        y = recty2;
                        x = static_cast<T>(_x1 + (static_cast<long double>(_x2 - _x1) * (y - _y1)) / (_y2 - _y1));
                    }
                    else if (outcode1 & 4)
                    {
                        x = rectx1;
                        y = static_cast<T>(_y1 + (static_cast<long double>(_y2 - _y1) * (x - _x1)) / (_x2 - _x1));
                    }
                    else
                    {
                        x = rectx2;
                        y = static_cast<T>(_y1 + (static_cast<long double>(_y2 - _y1) * (x - _x1)) / (_x2 - _x1));
                    }

                    _x1 = x;
                    _y1 = y;
                    outcode1 = computeOutCode(x, y);
                }
                else
                {
                    if (outcode2 & 2)
                    {
                        y = recty1;
                        x = static_cast<T>(_x1 + (static_cast<long double>(_x2 - _x1) * (y - _y1)) / (_y2 - _y1));
                    }
                    else if (outcode2 & 1)
                    {
                        y = recty2;
                        x = static_cast<T>(_x1 + (static_cast<long double>(_x2 - _x1) * (y - _y1)) / (_y2 - _y1));
                    }
                    else if (outcode2 & 4)
                    {
                        x = rectx1;
                        y = static_cast<T>(_y1 + (static_cast<long double>(_y2 - _y1) * (x - _x1)) / (_x2 - _x1));
                    }
                    else
                    {
                        x = rectx2;
                        y = static_cast<T>(_y1 + (static_cast<long double>(_y2 - _y1) * (x - _x1)) / (_x2 - _x1));
                    }
                    _x2 = x;
                    _y2 = y;
                    outcode2 = computeOutCode(x, y);
                }
            }

            x1 = _x1;
            y1 = _y1;
            x2 = _x2;
            y2 = _y2;
            return true;
        }

        constexpr bool operator==(Rect const& rect) const
        {
            return (x == rect.x && y == rect.y && width == rect.width && height == rect.height);
        }

        constexpr bool operator!=(Rect const& rect) const
        {
            return (x != rect.x || y != rect.y || width != rect.width || height != rect.height);
        }
    };

    template <Concepts::Arithmetic T>
    inline Rect<T> const Rect<T>::Empty = {T(0), T(0), T(0), T(0)};

    using RectFloat = Rect<float32>;
    using RectUInt = Rect<uint32>;
    using RectInt = Rect<int32>;

    inline RectFloat rectToFRect(RectInt const& r)
    {
        return RectFloat{
            static_cast<float32>(r.x),
            static_cast<float32>(r.y),
            static_cast<float32>(r.width),
            static_cast<float32>(r.height),
        };
    }
}
