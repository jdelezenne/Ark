#include "Ark/Math/Vector3.hpp"
#include "Ark/Math/Matrix3.hpp"

namespace Ark::Math
{
    const Vector3 Vector3::Zero = Vector3(0.0f, 0.0f, 0.0f);
    const Vector3 Vector3::One = Vector3(1.0f, 1.0f, 1.0f);
    const Vector3 Vector3::UnitX = Vector3(1.0f, 0.0f, 0.0f);
    const Vector3 Vector3::UnitY = Vector3(0.0f, 1.0f, 0.0f);
    const Vector3 Vector3::UnitZ = Vector3(0.0f, 0.0f, 1.0f);

    float32 Vector3::getLength() const
    {
        return Math::sqrt(x * x + y * y + z * z);
    }

    float32 Vector3::getLengthSquared() const
    {
        return (x * x + y * y + z * z);
    }

    float32 Vector3::getDistance(Vector3 const& a, Vector3 const& b)
    {
        return (a - b).getLength();
    }

    float32 Vector3::getDistanceSquared(Vector3 const& a, Vector3 const& b)
    {
        float32 const x = a.x - b.x;
        float32 const y = a.y - b.y;
        float32 const z = a.z - b.z;

        return (x * x + y * y + z * z);
    }

    void Vector3::normalize()
    {
        float32 const length = getLength();

        if (length >= Math::ZeroTolerance<float32>)
        {
            float32 const inverse = 1.0f / length;
            x *= inverse;
            y *= inverse;
            z *= inverse;
        }
    }

    Vector3 Vector3::getNormalized() const
    {
        Vector3 result = *this;
        result.normalize();
        return result;
    }

    float32 Vector3::getAngle(Vector3 const& a, Vector3 const& b)
    {
        Vector3 const crossProduct = cross(a, b);
        float32 const length = crossProduct.getLength();
        float32 const dotProduct = dot(a, b);

        return Math::arcTan2(length, dotProduct);
    }

    Vector3 Vector3::transform(Vector3 const& vector, Matrix3 const& transform)
    {
        return {
            vector.x * transform.m11 + vector.y * transform.m21 + vector.z * transform.m31,
            vector.x * transform.m12 + vector.y * transform.m22 + vector.z * transform.m32,
            vector.x * transform.m13 + vector.y * transform.m23 + vector.z * transform.m33,
        };
    }

    Vector3 Vector3::project(Vector3 const& vector, Vector3 const& normal)
    {
        float32 const squaredLength = dot(normal, normal);

        if (squaredLength < Math::ZeroTolerance<float32>)
        {
            return Zero;
        }

        return normal * dot(vector, normal) / squaredLength;
    }

    Vector3 Vector3::refract(Vector3 const& incident, Vector3 const& normal, float32 ratio)
    {
        float32 const cosTheta = incident.x * normal.x + incident.y * normal.y + incident.z * normal.z;
        float32 const k = 1.0f - ratio * ratio * (1.0f - cosTheta * cosTheta);

        if (k < Math::ZeroTolerance<float32>)
        {
            // Total internal reflection, no refraction.
            return Zero;
        }

        float32 const kSquared = Math::sqrt(k);

        return {
            ratio * incident.x - (ratio * cosTheta + kSquared) * normal.x,
            ratio * incident.y - (ratio * cosTheta + kSquared) * normal.y,
            ratio * incident.z - (ratio * cosTheta + kSquared) * normal.z,
        };
    }

    Vector3 Vector3::getBarycenter(Vector3 const& point, Vector3 const& a, Vector3 const& b, Vector3 const& c)
    {
        Vector3 const v0 = (b - a);
        Vector3 const v1 = (c - a);
        Vector3 const v2 = (point - a);

        float32 const d00 = (v0 | v0);
        float32 const d01 = (v0 | v1);
        float32 const d11 = (v1 | v1);
        float32 const d20 = (v2 | v0);
        float32 const d21 = (v2 | v1);

        float32 const denominator = (d00 * d11 - d01 * d01);

        Vector3 result;
        result.y = (d11 * d20 - d01 * d21) / denominator;
        result.z = (d00 * d21 - d01 * d20) / denominator;
        result.x = 1.0f - (result.z + result.y);
        return result;
    }
}
