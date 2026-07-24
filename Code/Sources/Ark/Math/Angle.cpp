#include "Ark/Math/Angle.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Math/Functions.hpp"

namespace Ark::Math
{
    float32 Angle::sine() const
    {
        return Math::sin(value);
    }

    float32 Angle::cosine() const
    {
        return Math::cos(value);
    }

    float32 Angle::tangent() const
    {
        return Math::tan(value);
    }

    Angle Angle::arcSine(float32 x)
    {
        return Angle(Math::arcSin(x));
    }

    Angle Angle::arcCosine(float32 x)
    {
        return Angle(Math::arcCos(x));
    }

    Angle Angle::arcTangent(float32 x)
    {
        return Angle(Math::arcTan(x));
    }

    Angle Angle::arcTangent2(float32 y, float32 x)
    {
        return Angle(Math::arcTan2(y, x));
    }

    void Angle::wrap()
    {
        float32 x = Math::mod(value, Math::TwoPi);
        if (x < 0.0f)
        {
            x += Math::TwoPi;
        }

        value = x;
    }

    Angle Angle::wrapped() const
    {
        Angle result = *this;
        result.wrap();
        return result;
    }

    void Angle::normalize()
    {
        value = Math::mod(value + Math::Pi, Math::TwoPi);
        if (value < 0.0f)
        {
            value += Math::TwoPi;
        }

        value -= Math::Pi;
    }

    Angle Angle::getNormalized() const
    {
        Angle result = *this;
        result.normalize();
        return result;
    }

    void Angle::normalize(Angle const min, Angle const max)
    {
        float32 const minimum = min.asRadians();
        float32 const maximum = max.asRadians();
        ARK_ASSERT_MSG(maximum > minimum, "Invalid angle range");

        float32 const range = maximum - minimum;
        float32 normalized = Math::mod(value - minimum, range);
        if (normalized < 0.0f)
        {
            normalized += range;
        }

        value = minimum + normalized;
    }

    Angle Angle::normalized(Angle const min, Angle const max) const
    {
        Angle result = *this;
        result.normalize(min, max);
        return result;
    }
}
