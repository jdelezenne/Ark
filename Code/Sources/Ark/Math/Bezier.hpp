#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector2.hpp"

namespace Ark::Math
{
    /// Evaluates a quadratic Bezier curve at parameter t.
    /// @tparam T The vector type (Vector2, Vector3, or scalar).
    /// @param p0 The start point (t=0).
    /// @param p1 The control point.
    /// @param p2 The end point (t=1).
    /// @param t The evaluation parameter, typically between 0 and 1.
    /// @return The point on the curve at parameter t.
    template <typename T>
    constexpr T evaluateBezierQuadratic(T const& p0, T const& p1, T const& p2, float32 t)
    {
        float32 const u = (1 - t);

        return u * u * p0 +
               2 * u * t * p1 +
               t * t * p2;
    }

    /// Evaluates a cubic Bezier curve at parameter t.
    /// @tparam T The vector type (Vector2, Vector3, or scalar).
    /// @param p0 The start point (t=0).
    /// @param p1 The first control point.
    /// @param p2 The second control point.
    /// @param p3 The end point (t=1).
    /// @param t The evaluation parameter, typically between 0 and 1.
    /// @return The point on the curve at parameter t.
    template <typename T>
    constexpr T evaluateBezierCubic(T const& p0, T const& p1, T const& p2, T const& p3, float32 t)
    {
        float32 const u = (1 - t);

        return u * u * u * p0 +
               3 * u * u * t * p1 +
               3 * u * t * t * p2 +
               t * t * t * p3;
    }

    /// A quadratic Bezier curve defined by three control points.
    /// @tparam T The vector type (Vector2 or Vector3).
    template <typename T>
    struct BezierQuadratic final
    {
    public:
        /// The start point of the curve.
        T p0;
        /// The control point influencing the curve shape.
        T p1;
        /// The end point of the curve.
        T p2;

    public:
        /// Constructs a quadratic Bezier curve with default control points.
        constexpr BezierQuadratic() = default;

        /// Constructs a quadratic Bezier curve with specified control points.
        /// @param p0 The start point.
        /// @param p1 The control point.
        /// @param p2 The end point.
        constexpr BezierQuadratic(T const& p0, T const& p1, T const& p2)
            : p0(p0)
            , p1(p1)
            , p2(p2)
        {
        }

        /// Evaluates the curve at parameter t.
        /// @param t The parameter value, typically between 0 and 1.
        /// @return The point on the curve.
        constexpr T evaluate(float32 t) const
        {
            return evaluateBezierQuadratic(p0, p1, p2, t);
        }

        /// Gets the start point.
        /// @return The start point.
        constexpr T getStart() const
        {
            return p0;
        }

        /// Sets the start point.
        /// @param value The new start point.
        constexpr void setStart(T const& value)
        {
            p0 = value;
        }

        /// Gets the control point.
        /// @return The control point.
        constexpr T getControl() const
        {
            return p1;
        }

        /// Sets the control point.
        /// @param value The new control point.
        constexpr void setControl(T const& value)
        {
            p1 = value;
        }

        /// Gets the end point.
        /// @return The end point.
        constexpr T getEnd() const
        {
            return p2;
        }

        /// Sets the end point.
        /// @param value The new end point.
        constexpr void setEnd(T const& value)
        {
            p2 = value;
        }

        /// Converts the curve to line segments (TODO).
        /// @return An array of line segment endpoints.
        Collections::Array<T> toLineSegments() const;

        /// Translates the curve by a value.
        /// @param value The translation amount.
        /// @return A new translated curve.
        constexpr BezierQuadratic operator+(T const& value) const
        {
            return {p0 + value, p1 + value, p2 + value};
        }

        /// Translates the curve by a negative value.
        /// @param value The translation amount (will be subtracted).
        /// @return A new translated curve.
        constexpr BezierQuadratic operator-(T const& value) const
        {
            return {p0 - value, p1 - value, p2 - value};
        }
    };

    /// A cubic Bezier curve defined by four control points.
    /// @tparam T The vector type (Vector2 or Vector3).
    template <typename T>
    struct BezierCubic final
    {
    public:
        /// The start point of the curve.
        T p0;
        /// The first control point influencing the curve shape.
        T p1;
        /// The second control point influencing the curve shape.
        T p2;
        /// The end point of the curve.
        T p3;

    public:
        /// Constructs a cubic Bezier curve with default control points.
        constexpr BezierCubic() = default;

        /// Constructs a cubic Bezier curve with specified control points.
        /// @param p0 The start point.
        /// @param p1 The first control point.
        /// @param p2 The second control point.
        /// @param p3 The end point.
        constexpr BezierCubic(T const& p0, T const& p1, T const& p2, T const& p3)
            : p0(p0)
            , p1(p1)
            , p2(p2)
            , p3(p3)
        {
        }

        /// Evaluates the curve at parameter t.
        /// @param t The parameter value, typically between 0 and 1.
        /// @return The point on the curve.
        constexpr T evaluate(float32 t) const
        {
            return evaluateBezierCubic(p0, p1, p2, p3, t);
        }

        /// Gets the start point.
        /// @return The start point.
        constexpr T getStart() const
        {
            return p0;
        }

        /// Sets the start point.
        /// @param value The new start point.
        constexpr void setStart(T const& value)
        {
            p0 = value;
        }

        /// Gets the first control point.
        /// @return The first control point.
        constexpr T getControl1() const
        {
            return p1;
        }

        /// Sets the first control point.
        /// @param value The new first control point.
        constexpr void setControl1(T const& value)
        {
            p1 = value;
        }

        /// Gets the second control point.
        /// @return The second control point.
        constexpr T getControl2() const
        {
            return p2;
        }

        /// Sets the second control point.
        /// @param value The new second control point.
        constexpr void setControl2(T const& value)
        {
            p2 = value;
        }

        /// Gets the end point.
        /// @return The end point.
        constexpr T getEnd() const
        {
            return p3;
        }

        /// Sets the end point.
        /// @param value The new end point.
        constexpr void setEnd(T const& value)
        {
            p3 = value;
        }

        /// Converts the curve to line segments (TODO).
        /// @return An array of line segment endpoints.
        Collections::Array<T> toLineSegments() const;

        /// Translates the curve by a value.
        /// @param value The translation amount.
        /// @return A new translated curve.
        constexpr BezierCubic operator+(T const& value) const
        {
            return {p0 + value, p1 + value, p2 + value, p3 + value};
        }

        /// Translates the curve by a negative value.
        /// @param value The translation amount (will be subtracted).
        /// @return A new translated curve.
        constexpr BezierCubic operator-(T const& value) const
        {
            return {p0 - value, p1 - value, p2 - value, p3 - value};
        }
    };
}
