#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Types.hpp"

namespace Ark::Math
{
    /// Represents an angle in different units of measure with convertion across units.
    /// @note
    /// - The angle is stored in radians.
    /// - The implicit convertion to floating-point values is in radians.
    /// - The operations with floating-point values are handled in radians.
    struct Angle final
    {
    public:
        /// Specifies the units of measure of an angle.
        enum class Unit
        {
            Radians, ///< Radians
            Degrees, ///< Degrees
        };

    private:
        float32 value; ///< Angle in radians.

    public:
        static Angle const Zero;

        constexpr Angle() = default;

        /// Constructs an angle from a value in the specified unit.
        /// @param value The angle value.
        /// @param unit The unit of the value (default: radians).
        constexpr Angle(float32 const value, Unit const unit = Unit::Radians)
        {
            switch (unit)
            {
                case Unit::Radians:
                    this->value = value;
                    break;

                case Unit::Degrees:
                    setDegrees(value);
                    break;

                default:
                    ARK_ASSERT_FAIL();
                    break;
            }
        }

        /// Implicitly converts to a float32 value in radians.
        constexpr operator float32()
        {
            return value;
        }

        /// Constructs an angle from a value in radians.
        /// @param radians The angle in radians.
        /// @return The constructed Angle.
        static constexpr Angle fromRadians(float32 const radians)
        {
            return Angle(radians, Unit::Radians);
        }

        /// Constructs an angle from a value in degrees.
        /// @param degrees The angle in degrees.
        /// @return The constructed Angle.
        static constexpr Angle fromDegrees(float32 const degrees)
        {
            return Angle(degrees, Unit::Degrees);
        }

        /// Returns the angle value in the specified unit.
        /// @param unit The desired output unit.
        /// @return The angle value in the given unit.
        constexpr float32 as(Unit const unit) const
        {
            switch (unit)
            {
                case Unit::Radians:
                    return value;

                case Unit::Degrees:
                    return asDegrees();

                default:
                    ARK_ASSERT_FAIL();
                    return value;
            }
        }

        /// Returns the angle value in radians.
        constexpr float32 asRadians() const
        {
            return value;
        }

        /// Returns the angle value in degrees.
        constexpr float32 asDegrees() const
        {
            return Math::radiansToDegrees(value);
        }

        /// Sets the angle value from radians.
        /// @param radians The new value in radians.
        constexpr void setRadians(float32 const radians)
        {
            value = radians;
        }

        /// Sets the angle value from degrees.
        /// @param degrees The new value in degrees.
        constexpr void setDegrees(float32 const degrees)
        {
            value = Math::degreesToRadians(degrees);
        }

        /// Returns the sine of this angle.
        float32 sine() const;

        /// Returns the cosine of this angle.
        float32 cosine() const;

        /// Returns the tangent of this angle.
        float32 tangent() const;

        /// Returns the arc-sine of x as an Angle.
        /// @param x The input value in [-1, 1].
        static Angle arcSine(float32 x);

        /// Returns the arc-cosine of x as an Angle.
        /// @param x The input value in [-1, 1].
        static Angle arcCosine(float32 x);

        /// Returns the arc-tangent of x as an Angle.
        /// @param x The input value.
        static Angle arcTangent(float32 x);

        /// Returns the arc-tangent of y/x as an Angle, using the signs of both arguments to determine the quadrant.
        /// @param y The y value.
        /// @param x The x value.
        static Angle arcTangent2(float32 y, float32 x);

        /// Wraps the angle into [0, 2π) (i.e. [0, 360) degrees).
        void wrap();

        /// Returns a copy of this angle wrapped into [0, 2π).
        Angle wrapped() const;

        /// Wraps the angle into [-π, π) (i.e. [-180, 180) degrees).
        void normalize();

        /// Returns a copy of this angle wrapped into [-π, π).
        Angle getNormalized() const;

        /// Wraps the angle into the range [min, max].
        /// @param min The minimum bound.
        /// @param max The maximum bound.
        void normalize(Angle const min, Angle const max);

        /// Returns a copy of this angle wrapped into the range [min, max].
        /// @param min The minimum bound.
        /// @param max The maximum bound.
        Angle normalized(Angle const min, Angle const max) const;

        constexpr Angle operator-(Angle const right) const
        {
            return Angle::fromDegrees(this->asDegrees() - right.asDegrees());
        }

        constexpr bool operator==(Angle const other)
        {
            return asDegrees() == other.asDegrees();
        }

        constexpr bool operator!=(Angle const other)
        {
            return asDegrees() != other.asDegrees();
        }
    };

    inline constexpr Angle Angle::Zero(0.0f, Unit::Radians);

    static constexpr bool operator<(Angle const left, Angle const right)
    {
        return left.asDegrees() < right.asDegrees();
    }

    static constexpr bool operator>(Angle const left, Angle const right)
    {
        return left.asDegrees() > right.asDegrees();
    }

    static constexpr bool operator<=(Angle const left, Angle const right)
    {
        return left.asDegrees() <= right.asDegrees();
    }

    static constexpr bool operator>=(Angle const left, Angle const right)
    {
        return left.asDegrees() >= right.asDegrees();
    }

    static constexpr Angle operator+(Angle const left, Angle const right)
    {
        return Angle::fromDegrees(left.asDegrees() + right.asDegrees());
    }

    static constexpr Angle& operator+=(Angle& left, Angle const right)
    {
        return left = left + right;
    }

    static constexpr Angle operator-(Angle const left, Angle const right)
    {
        return Angle::fromDegrees(left.asDegrees() - right.asDegrees());
    }

    static constexpr Angle& operator-=(Angle& left, Angle const right)
    {
        return left = Angle::fromDegrees(left.asDegrees() - right.asDegrees());
    }

    static constexpr Angle operator*(Angle const left, float right)
    {
        return Angle::fromDegrees(left.asDegrees() * right);
    }

    static constexpr Angle operator*(float left, Angle const right)
    {
        return right * left;
    }

    static constexpr Angle& operator*=(Angle& left, float right)
    {
        return left = left * right;
    }

    static constexpr Angle operator/(Angle const left, float right)
    {
        return Angle::fromDegrees(left.asDegrees() / right);
    }

    static constexpr Angle& operator/=(Angle& left, float right)
    {
        return left = left / right;
    }

    static constexpr float operator/(Angle const left, Angle const right)
    {
        return left.asDegrees() / right.asDegrees();
    }

    static constexpr Angle operator""_rad(float128 const radians)
    {
        return Angle::fromRadians(static_cast<float32>(radians));
    }

    static constexpr Angle operator""_rad(uint64 const radians)
    {
        return Angle::fromRadians(static_cast<float32>(radians));
    }

    constexpr Angle operator""_deg(float128 const degrees)
    {
        return Angle::fromDegrees(static_cast<float32>(degrees));
    }

    constexpr Angle operator""_deg(uint64 const degrees)
    {
        return Angle::fromDegrees(static_cast<float32>(degrees));
    }
}
