#include "Ark/Math/Quaternion.hpp"
#include "Ark/Math/Matrix4.hpp"

namespace Ark::Math
{
    const Quaternion Quaternion::Zero = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
    const Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

    float32 Quaternion::getLength()
    {
        return Math::sqrt(x * x + y * y + z * z + w * w);
    }

    float32 Quaternion::getLengthSquared()
    {
        return (x * x + y * y + z * z + w * w);
    }

    void Quaternion::normalize()
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

    Quaternion Quaternion::getNormalized() const
    {
        Quaternion result = *this;
        result.normalize();
        return result;
    }

    void Quaternion::invert()
    {
        float32 const lengthSquared = getLengthSquared();

        if (lengthSquared >= Math::ZeroTolerance<float32>)
        {
            float32 const inverse = 1.0f / lengthSquared;
            x *= -inverse;
            y *= -inverse;
            z *= -inverse;
            w *= inverse;
        }
    }

    Quaternion Quaternion::getInversed() const
    {
        Quaternion result = *this;
        result.invert();
        return result;
    }

    void Quaternion::conjugate()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    Quaternion Quaternion::getConjugate() const
    {
        return Quaternion(-x, -y, -z, w);
    }

    float32 Quaternion::dot(Quaternion const& a, Quaternion const& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    float32 Quaternion::dot(Quaternion const& other) const
    {
        return dot(*this, other);
    }

    float32 Quaternion::angle(Quaternion const& a, Quaternion const& b)
    {
        float32 d = dot(a, b);
        d = Math::clamp(d, -1.0f, 1.0f);
        return Math::arcCos(Math::abs(d)) * 2.0f;
    }

    Quaternion Quaternion::fromAxisAngle(Vector3 const& axis, float32 angle)
    {
        Vector3 normalizedAxis = axis.getNormalized();
        float32 halfAngle = angle * 0.5f;
        float32 sinHalfAngle = Math::sin(halfAngle);

        return Quaternion(
            normalizedAxis.x * sinHalfAngle,
            normalizedAxis.y * sinHalfAngle,
            normalizedAxis.z * sinHalfAngle,
            Math::cos(halfAngle));
    }

    Quaternion Quaternion::fromEulerAngles(float32 pitch, float32 yaw, float32 roll)
    {
        float32 halfPitch = pitch * 0.5f; // X
        float32 halfYaw = yaw * 0.5f;     // Y
        float32 halfRoll = roll * 0.5f;   // Z

        float32 sp = Math::sin(halfPitch);
        float32 cp = Math::cos(halfPitch);
        float32 sy = Math::sin(halfYaw);
        float32 cy = Math::cos(halfYaw);
        float32 sr = Math::sin(halfRoll);
        float32 cr = Math::cos(halfRoll);

        Quaternion qx(sp, 0.0f, 0.0f, cp);
        Quaternion qy(0.0f, sy, 0.0f, cy);
        Quaternion qz(0.0f, 0.0f, sr, cr);

        Quaternion q = multiply(multiply(qy, qx), qz);
        return q.getNormalized();
    }

    Quaternion Quaternion::lookRotation(Vector3 const& forward, Vector3 const& up)
    {
        Vector3 f = forward.getNormalized();
        Vector3 r = Vector3::cross(up, f).getNormalized();
        Vector3 u = Vector3::cross(f, r);

        Matrix4 m = Matrix4::rotation(r, u, f);
        return fromMatrix(m);
    }

    Quaternion Quaternion::fromMatrix(const Matrix4& m)
    {
        Quaternion q;
        float32 trace = m.m11 + m.m22 + m.m33;

        if (trace > 0.0f)
        {
            float32 s = 0.5f / Math::sqrt(trace + 1.0f);
            q.w = 0.25f / s;
            q.x = (m.m32 - m.m23) * s;
            q.y = (m.m13 - m.m31) * s;
            q.z = (m.m21 - m.m12) * s;
        }
        else
        {
            if (m.m11 > m.m22 && m.m11 > m.m33)
            {
                float32 s = 2.0f * Math::sqrt(1.0f + m.m11 - m.m22 - m.m33);
                q.w = (m.m32 - m.m23) / s;
                q.x = 0.25f * s;
                q.y = (m.m12 + m.m21) / s;
                q.z = (m.m13 + m.m31) / s;
            }
            else if (m.m22 > m.m33)
            {
                float32 s = 2.0f * Math::sqrt(1.0f + m.m22 - m.m11 - m.m33);
                q.w = (m.m13 - m.m31) / s;
                q.x = (m.m12 + m.m21) / s;
                q.y = 0.25f * s;
                q.z = (m.m23 + m.m32) / s;
            }
            else
            {
                float32 s = 2.0f * Math::sqrt(1.0f + m.m33 - m.m11 - m.m22);
                q.w = (m.m21 - m.m12) / s;
                q.x = (m.m13 + m.m31) / s;
                q.y = (m.m23 + m.m32) / s;
                q.z = 0.25f * s;
            }
        }

        return q.getNormalized();
    }

    Matrix4 Quaternion::toMatrix() const
    {
        return Matrix4::fromQuaternion(*this);
    }

    Vector3 Quaternion::transformVector(Vector3 const& v, Quaternion const& q)
    {
        return q.transformVector(v);
    }

    Vector3 Quaternion::transformVector(Vector3 const& v) const
    {
        // Compute q * v * q^-1
        Vector3 const qvec(x, y, z);
        Vector3 uv = Vector3::cross(qvec, v);
        Vector3 uuv = Vector3::cross(qvec, uv);

        uv = uv * (2.0f * w);
        uuv = uuv * 2.0f;

        return v + uv + uuv;
    }

    Quaternion Quaternion::lerp(Quaternion const& start, Quaternion const& end, float32 amount)
    {
        float32 d = dot(start, end);

        Quaternion result;
        if (d < 0.0f)
        {
            result = Quaternion(
                Math::lerp(start.x, -end.x, amount),
                Math::lerp(start.y, -end.y, amount),
                Math::lerp(start.z, -end.z, amount),
                Math::lerp(start.w, -end.w, amount));
        }
        else
        {
            result = Quaternion(
                Math::lerp(start.x, end.x, amount),
                Math::lerp(start.y, end.y, amount),
                Math::lerp(start.z, end.z, amount),
                Math::lerp(start.w, end.w, amount));
        }

        return result.getNormalized();
    }

    Quaternion Quaternion::slerp(Quaternion const& start, Quaternion const& end, float32 amount)
    {
        float32 d = dot(start, end);

        Quaternion endAdjusted = end;
        if (d < 0.0f)
        {
            endAdjusted = Quaternion(-end.x, -end.y, -end.z, -end.w);
            d = -d;
        }

        if (d > 0.9995f)
        {
            return lerp(start, endAdjusted, amount);
        }

        d = Math::clamp(d, -1.0f, 1.0f);
        float32 theta = Math::arcCos(d) * amount;

        Quaternion relative = endAdjusted - start * d;
        relative.normalize();

        return start * Math::cos(theta) + relative * Math::sin(theta);
    }

    Quaternion Quaternion::smoothStep(Quaternion const& start, Quaternion const& end, float32 amount)
    {
        amount = Math::smoothStep(amount);
        return slerp(start, end, amount);
    }

    Quaternion Quaternion::multiply(Quaternion const& a, Quaternion const& b)
    {
        return {
            a.x * b.w + a.w * b.x + a.y * b.z - a.z * b.y,
            a.y * b.w + a.w * b.y + a.z * b.x - a.x * b.z,
            a.z * b.w + a.w * b.z + a.x * b.y - a.y * b.x,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
    }
}
