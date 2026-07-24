#include "Ark/Math/Vector4.hpp"

namespace Ark::Math
{
    const Vector4 Vector4::Zero = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::One = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    const Vector4 Vector4::UnitX = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::UnitY = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::UnitZ = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    const Vector4 Vector4::UnitW = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

    float32 Vector4::getLength()
    {
        return Math::sqrt(x * x + y * y + z * z + w * w);
    }

    float32 Vector4::distance(Vector4 const& a, Vector4 const& b)
    {
        return (a - b).getLength();
    }

    float32 Vector4::distanceSquared(Vector4 const& a, Vector4 const& b)
    {
        float32 const x = a.x - b.x;
        float32 const y = a.y - b.y;
        float32 const z = a.z - b.z;
        float32 const w = a.w - b.w;

        return (x * x + y * y + z * z + w * w);
    }

    void Vector4::normalize()
    {
        float32 const length = getLength();

        if (length >= Math::ZeroTolerance<float32>)
        {
            float32 const inverse = 1.0f / length;
            x *= inverse;
            y *= inverse;
            z *= inverse;
            w *= inverse;
        }
    }

    Vector4 Vector4::getNormalized() const
    {
        Vector4 result = *this;
        result.normalize();
        return result;
    }
}
