#include "Ark/Math/Matrix4.hpp"
#include "Ark/Math/Matrix3.hpp"
#include "Ark/Math/Quaternion.hpp"

namespace Ark::Math
{
    // clang-format off
    inline constexpr Matrix4 Matrix4::Zero = Matrix4(
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f);

    inline constexpr Matrix4 Matrix4::Identity = Matrix4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    // clang-format on

    Matrix4::Matrix4(Matrix3 const& matrix)
    {
        Memory::copy(&matrix.m11, &m11, sizeof(Vector3));
        Memory::copy(&matrix.m21, &m21, sizeof(Vector3));
        Memory::copy(&matrix.m31, &m31, sizeof(Vector3));
        setColumn(3, Vector4::UnitW);
        setRow(3, Vector4::UnitW);
    }

    float32 Matrix4::getDeterminant() const
    {
        return m11 * (m22 * (m33 * m44 - m34 * m43) - m23 * (m32 * m44 - m34 * m42) + m24 * (m32 * m43 - m33 * m42)) -
               m12 * (m21 * (m33 * m44 - m34 * m43) - m23 * (m31 * m44 - m34 * m41) + m24 * (m31 * m43 - m33 * m41)) +
               m13 * (m21 * (m32 * m44 - m34 * m42) - m22 * (m31 * m44 - m34 * m41) + m24 * (m31 * m42 - m32 * m41)) -
               m14 * (m21 * (m32 * m43 - m33 * m42) - m22 * (m31 * m43 - m33 * m41) + m23 * (m31 * m42 - m32 * m41));
    }

    Matrix4 Matrix4::transposed(Matrix4 const& matrix)
    {
        Matrix4 result = matrix;
        result.transpose();
        return result;
    }

    void Matrix4::transpose()
    {
        Matrix4 result;
        result.m11 = m11;
        result.m12 = m21;
        result.m13 = m31;
        result.m14 = m41;
        result.m21 = m12;
        result.m22 = m22;
        result.m23 = m32;
        result.m24 = m42;
        result.m31 = m13;
        result.m32 = m23;
        result.m33 = m33;
        result.m34 = m43;
        result.m41 = m14;
        result.m42 = m24;
        result.m43 = m34;
        result.m44 = m44;
        *this = result;
    }

    Matrix4 Matrix4::getTransposed() const
    {
        Matrix4 result = *this;
        result.transpose();
        return result;
    }

    Matrix4 Matrix4::inversed(Matrix4 const& matrix)
    {
        Matrix4 result = matrix;
        result.inverse();
        return result;
    }

    Outcome Matrix4::inverse()
    {
        float32 const a0 = values[0] * values[5] - values[1] * values[4];
        float32 const a1 = values[0] * values[6] - values[2] * values[4];
        float32 const a2 = values[0] * values[7] - values[3] * values[4];
        float32 const a3 = values[1] * values[6] - values[2] * values[5];
        float32 const a4 = values[1] * values[7] - values[3] * values[5];
        float32 const a5 = values[2] * values[7] - values[3] * values[6];
        float32 const b0 = values[8] * values[13] - values[9] * values[12];
        float32 const b1 = values[8] * values[14] - values[10] * values[12];
        float32 const b2 = values[8] * values[15] - values[11] * values[12];
        float32 const b3 = values[9] * values[14] - values[10] * values[13];
        float32 const b4 = values[9] * values[15] - values[11] * values[13];
        float32 const b5 = values[10] * values[15] - values[11] * values[14];

        // Calculate the determinant.
        float32 const determinant = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

        if (Math::isZero(determinant))
        {
            return makeError();
        }

        float32 const inverseDeterminant = 1.0f / determinant;

        Matrix4 result;
        result.values[0] = values[5] * b5 - values[6] * b4 + values[7] * b3;
        result.values[1] = -values[1] * b5 + values[2] * b4 - values[3] * b3;
        result.values[2] = values[13] * a5 - values[14] * a4 + values[15] * a3;
        result.values[3] = -values[9] * a5 + values[10] * a4 - values[11] * a3;

        result.values[4] = -values[4] * b5 + values[6] * b2 - values[7] * b1;
        result.values[5] = values[0] * b5 - values[2] * b2 + values[3] * b1;
        result.values[6] = -values[12] * a5 + values[14] * a2 - values[15] * a1;
        result.values[7] = values[8] * a5 - values[10] * a2 + values[11] * a1;

        result.values[8] = values[4] * b4 - values[5] * b2 + values[7] * b0;
        result.values[9] = -values[0] * b4 + values[1] * b2 - values[3] * b0;
        result.values[10] = values[12] * a4 - values[13] * a2 + values[15] * a0;
        result.values[11] = -values[8] * a4 + values[9] * a2 - values[11] * a0;

        result.values[12] = -values[4] * b3 + values[5] * b1 - values[6] * b0;
        result.values[13] = values[0] * b3 - values[1] * b1 + values[2] * b0;
        result.values[14] = -values[12] * a3 + values[13] * a1 - values[14] * a0;
        result.values[15] = values[8] * a3 - values[9] * a1 + values[10] * a0;

        *this = multiply(result, inverseDeterminant);

        return makeOutcome();
    }

    Matrix4 Matrix4::getInversed() const
    {
        Matrix4 result = *this;
        result.inverse();
        return result;
    }

    Matrix4 Matrix4::translation(Vector3 const& translation)
    {
        Matrix4 result = Matrix4::Identity;
        result.m41 = translation.x;
        result.m42 = translation.y;
        result.m43 = translation.z;
        return result;
    }

    Vector3 Matrix4::getTranslation() const
    {
        return {m41, m42, m43};
    }

    void Matrix4::setTranslation(Vector3 const& translation)
    {
        m41 = translation.x;
        m42 = translation.y;
        m43 = translation.z;
    }

    void Matrix4::translate(Vector3 const& position)
    {
        m41 += position.x;
        m42 += position.y;
        m43 += position.z;
    }

    Matrix4 Matrix4::rotationXLH(float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        Matrix4 result = Matrix4::Identity;
        result.m22 = cosine;
        result.m23 = sine;
        result.m32 = -sine;
        result.m33 = cosine;
        return result;
    }

    Matrix4 Matrix4::rotationXRH(float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        Matrix4 result = Matrix4::Identity;
        result.m22 = cosine;
        result.m23 = -sine;
        result.m32 = sine;
        result.m33 = cosine;
        return result;
    }

    Matrix4 Matrix4::rotationX(float32 angle)
    {
        if constexpr (CurrentCoordinateSystem == CoordinateSystem::LeftHanded)
        {
            return rotationXLH(angle);
        }
        else
        {
            return rotationXRH(angle);
        }
    }

    Matrix4 Matrix4::rotationYLH(float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        Matrix4 result = Matrix4::Identity;
        result.m11 = cosine;
        result.m13 = -sine;
        result.m31 = sine;
        result.m33 = cosine;
        return result;
    }

    Matrix4 Matrix4::rotationYRH(float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        Matrix4 result = Matrix4::Identity;
        result.m11 = cosine;
        result.m13 = sine;
        result.m31 = -sine;
        result.m33 = cosine;
        return result;
    }

    Matrix4 Matrix4::rotationY(float32 angle)
    {
        if constexpr (CurrentCoordinateSystem == CoordinateSystem::LeftHanded)
        {
            return rotationYLH(angle);
        }
        else
        {
            return rotationYRH(angle);
        }
    }

    Matrix4 Matrix4::rotationZLH(float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        Matrix4 result = Matrix4::Identity;
        result.m11 = cosine;
        result.m12 = sine;
        result.m21 = -sine;
        result.m22 = cosine;
        return result;
    }

    Matrix4 Matrix4::rotationZRH(float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        Matrix4 result = Matrix4::Identity;
        result.m11 = cosine;
        result.m12 = -sine;
        result.m21 = sine;
        result.m22 = cosine;
        return result;
    }

    Matrix4 Matrix4::rotationZ(float32 angle)
    {
        if constexpr (CurrentCoordinateSystem == CoordinateSystem::LeftHanded)
        {
            return rotationZLH(angle);
        }
        else
        {
            return rotationZRH(angle);
        }
    }

    Matrix4 Matrix4::rotationAxisLH(Vector3 const& axis, float32 angle)
    {
        float32 const x = axis.x;
        float32 const y = axis.y;
        float32 const z = axis.z;
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);
        float32 const xx = x * x;
        float32 const yy = y * y;
        float32 const zz = z * z;
        float32 const xy = x * y;
        float32 const xz = x * z;
        float32 const yz = y * z;

        Matrix4 result = Matrix4::Identity;
        result.m11 = xx + cosine * (1.0f - xx);
        result.m12 = xy - cosine * xy + sine * z;
        result.m13 = xz - cosine * xz - sine * y;
        result.m21 = xy - cosine * xy - sine * z;
        result.m22 = yy + cosine * (1.0f - yy);
        result.m23 = yz - cosine * yz + sine * x;
        result.m31 = xz - cosine * xz + sine * y;
        result.m32 = yz - cosine * yz - sine * x;
        result.m33 = zz + cosine * (1.0f - zz);
        return result;
    }

    Matrix4 Matrix4::rotationAxisRH(Vector3 const& axis, float32 angle)
    {
        float32 const x = axis.x;
        float32 const y = axis.y;
        float32 const z = axis.z;
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);
        float32 const xx = x * x;
        float32 const yy = y * y;
        float32 const zz = z * z;
        float32 const xy = x * y;
        float32 const xz = x * z;
        float32 const yz = y * z;

        Matrix4 result = Matrix4::Identity;
        result.m11 = xx + cosine * (1.0f - xx);
        result.m12 = xy - cosine * xy - sine * z;
        result.m13 = xz - cosine * xz + sine * y;
        result.m21 = xy - cosine * xy + sine * z;
        result.m22 = yy + cosine * (1.0f - yy);
        result.m23 = yz - cosine * yz - sine * x;
        result.m31 = xz - cosine * xz - sine * y;
        result.m32 = yz - cosine * yz + sine * x;
        result.m33 = zz + cosine * (1.0f - zz);
        return result;
    }

    Matrix4 Matrix4::rotationAxis(Vector3 const& axis, float32 angle)
    {
        if constexpr (CurrentCoordinateSystem == CoordinateSystem::LeftHanded)
        {
            return rotationAxisLH(axis, angle);
        }
        else
        {
            return rotationAxisRH(axis, angle);
        }
    }

    Matrix4 Matrix4::scaling(Vector3 const& scale)
    {
        Matrix4 result = Matrix4::Identity;
        result.m11 = scale.x;
        result.m22 = scale.y;
        result.m33 = scale.z;
        return result;
    }

    Matrix4 Matrix4::scaling(float32 scale)
    {
        Matrix4 result = Matrix4::Identity;
        result.m11 = scale;
        result.m22 = scale;
        result.m33 = scale;
        return result;
    }

    Vector3 Matrix4::getScale() const
    {
        return {m11, m22, m33};
    }

    void Matrix4::setScale(Vector3 const& scale)
    {
        m11 = scale.x;
        m22 = scale.y;
        m33 = scale.z;
    }

    void Matrix4::scale(Vector3 const& scale)
    {
        *this = multiply(*this, scaling(scale));
    }

    Matrix4 Matrix4::orthographic(float32 width, float32 height, float32 zNear, float32 zFar)
    {
        float32 const halfWidth = width * 0.5f;
        float32 const halfHeight = height * 0.5f;

        return orthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
    }

    Matrix4 Matrix4::orthographicOffCenter(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
    {
        float32 const zRange = 1.0f / (zFar - zNear);

        Matrix4 result = Matrix4::Identity;
        result.m11 = 2.0f / (right - left);
        result.m22 = 2.0f / (top - bottom);
        result.m33 = zRange;
        result.m41 = (left + right) / (left - right);
        result.m42 = (top + bottom) / (bottom - top);
        result.m43 = -zNear * zRange;
        return result;
    }

    Matrix4 Matrix4::frustum(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
    {
        Matrix4 result = Matrix4::Zero;
        result.m11 = (2.0f * zNear) / (right - left);
        result.m22 = (2.0f * zNear) / (top - bottom);
        result.m31 = (left + right) / (left - right);
        result.m32 = (top + bottom) / (bottom - top);
        result.m33 = -(zFar + zNear) / (zFar - zNear);
        result.m34 = -1.0f;
        result.m42 = 0.0f;
        result.m43 = (-2.0f * zFar * zNear) / (zFar - zNear);
        return result;
    }

    Matrix4 Matrix4::perspectiveLH(float32 width, float32 height, float32 zNear, float32 zFar)
    {
        float32 const halfWidth = width * 0.5f;
        float32 const halfHeight = height * 0.5f;

        return perspectiveOffCenterLH(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
    }

    Matrix4 Matrix4::perspectiveOffCenterLH(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
    {
        float32 const zRange = zFar / (zFar - zNear);

        Matrix4 result = Matrix4::Zero;

        result.m11 = 2.0f * zNear / (right - left);
        result.m22 = 2.0f * zNear / (top - bottom);
        result.m31 = (right + left) / (right - left);
        result.m32 = (top + bottom) / (top - bottom);
        result.m33 = zRange;
        result.m34 = 1.0f;
        result.m43 = -zNear * zRange;
        result.m44 = 0.0f;

        return result;
    }

    Matrix4 Matrix4::perspectiveRH(float32 width, float32 height, float32 zNear, float32 zFar)
    {
        float32 const halfWidth = width * 0.5f;
        float32 const halfHeight = height * 0.5f;

        return perspectiveOffCenterRH(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
    }

    Matrix4 Matrix4::perspectiveOffCenterRH(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
    {
        float32 const zRange = zFar / (zNear - zFar);

        Matrix4 result = Matrix4::Zero;

        result.m11 = 2.0f * zNear / (right - left);
        result.m22 = 2.0f * zNear / (top - bottom);
        result.m31 = (right + left) / (right - left);
        result.m32 = (top + bottom) / (top - bottom);
        result.m33 = zRange;
        result.m34 = -1.0f;
        result.m43 = zNear * zRange;
        result.m44 = 0.0f;

        return result;
    }

    Matrix4 Matrix4::perspectiveFieldOfViewLH(float32 fieldOfView, float32 aspectRatio, float32 zNear, float32 zFar)
    {
        float32 const yScale = 1.0f / Math::tan(fieldOfView * 0.5f);
        float32 const xScale = yScale / aspectRatio;

        float32 const halfWidth = zNear / xScale;
        float32 const halfHeight = zNear / yScale;

        return perspectiveOffCenterLH(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
    }

    Matrix4 Matrix4::perspectiveFieldOfViewRH(float32 fieldOfView, float32 aspectRatio, float32 zNear, float32 zFar)
    {
        float32 const yScale = 1.0f / Math::tan(fieldOfView * 0.5f);
        float32 const xScale = yScale / aspectRatio;

        float32 const halfWidth = zNear / xScale;
        float32 const halfHeight = zNear / yScale;

        return perspectiveOffCenterRH(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
    }

    Matrix4 Matrix4::lookAtLH(Vector3 const& eye, Vector3 const& target, Vector3 const& up)
    {
        Vector3 const zaxis = (target - eye).getNormalized();
        Vector3 const xaxis = Vector3::cross(up, zaxis).getNormalized();
        Vector3 const yaxis = Vector3::cross(zaxis, xaxis);

        Matrix4 result;

        result.m11 = xaxis.x;
        result.m12 = xaxis.y;
        result.m13 = xaxis.z;

        result.m21 = yaxis.x;
        result.m22 = yaxis.y;
        result.m23 = yaxis.z;

        result.m31 = zaxis.x;
        result.m32 = zaxis.y;
        result.m33 = zaxis.z;

        result.m14 = 0.0f;
        result.m24 = 0.0f;
        result.m34 = 0.0f;

        result.m41 = -Vector3::dot(xaxis, eye);
        result.m42 = -Vector3::dot(yaxis, eye);
        result.m43 = -Vector3::dot(zaxis, eye);
        result.m44 = 1.0f;

        return result;
    }

    Matrix4 Matrix4::lookAtRH(Vector3 const& eye, Vector3 const& target, Vector3 const& up)
    {
        Vector3 const zaxis = (eye - target).getNormalized();
        Vector3 const xaxis = Vector3::cross(up, zaxis).getNormalized();
        Vector3 const yaxis = Vector3::cross(zaxis, xaxis);

        Matrix4 result;

        result.m11 = xaxis.x;
        result.m12 = xaxis.y;
        result.m13 = xaxis.z;

        result.m21 = yaxis.x;
        result.m22 = yaxis.y;
        result.m23 = yaxis.z;

        result.m31 = zaxis.x;
        result.m32 = zaxis.y;
        result.m33 = zaxis.z;

        result.m14 = 0.0f;
        result.m24 = 0.0f;
        result.m34 = 0.0f;

        result.m41 = -Vector3::dot(xaxis, eye);
        result.m42 = -Vector3::dot(yaxis, eye);
        result.m43 = -Vector3::dot(zaxis, eye);
        result.m44 = 1.0f;

        return result;
    }

    Matrix4 Matrix4::rotation(Vector3 const& right, Vector3 const& up, Vector3 const& forward)
    {
        return {
            // Row 0: Right vector (X-axis)
            right.x,
            right.y,
            right.z,
            0.0f,

            // Row 1: Up vector (Y-axis)
            up.x,
            up.y,
            up.z,
            0.0f,

            // Row 2: Forward vector (Z-axis)
            forward.x,
            forward.y,
            forward.z,
            0.0f,

            // Row 3: Translation
            0.0f,
            0.0f,
            0.0f,
            1.0f,
        };
    }

    Vector3 Matrix4::toEulerAngles(Matrix4 const& m)
    {
        Vector3 angles;

        // Pitch (x), Yaw (y), Roll (z)
        if (Math::abs(m.m31) < 1.0f)
        {
            angles.x = Math::arcSin(-m.m31);        // pitch
            angles.y = Math::arcTan2(m.m21, m.m11); // yaw
            angles.z = Math::arcTan2(m.m32, m.m33); // roll
        }
        else
        {
            // Gimbal lock case
            angles.x = (m.m31 <= -1.0f) ? Math::PiOver2 : -Math::PiOver2;
            angles.y = Math::arcTan2(-m.m12, m.m22);
            angles.z = 0.0f;
        }

        return angles;
    }

    Matrix4 Matrix4::fromQuaternion(const Quaternion& q)
    {
        Matrix4 result;

        float32 const xx = q.x * q.x;
        float32 const yy = q.y * q.y;
        float32 const zz = q.z * q.z;
        float32 const xy = q.x * q.y;
        float32 const xz = q.x * q.z;
        float32 const yz = q.y * q.z;
        float32 const wx = q.w * q.x;
        float32 const wy = q.w * q.y;
        float32 const wz = q.w * q.z;

        result.m11 = 1.0f - 2.0f * (yy + zz);
        result.m12 = 2.0f * (xy - wz);
        result.m13 = 2.0f * (xz + wy);
        result.m14 = 0.0f;

        result.m21 = 2.0f * (xy + wz);
        result.m22 = 1.0f - 2.0f * (xx + zz);
        result.m23 = 2.0f * (yz - wx);
        result.m24 = 0.0f;

        result.m31 = 2.0f * (xz - wy);
        result.m32 = 2.0f * (yz + wx);
        result.m33 = 1.0f - 2.0f * (xx + yy);
        result.m34 = 0.0f;

        result.m41 = 0.0f;
        result.m42 = 0.0f;
        result.m43 = 0.0f;
        result.m44 = 1.0f;

        return result;
    }

    Matrix4 Matrix4::multiply(Matrix4 const& left, float32 right)
    {
        return {
            left.m11 * right,
            left.m12 * right,
            left.m13 * right,
            left.m14 * right,
            left.m21 * right,
            left.m22 * right,
            left.m23 * right,
            left.m24 * right,
            left.m31 * right,
            left.m32 * right,
            left.m33 * right,
            left.m34 * right,
            left.m41 * right,
            left.m42 * right,
            left.m43 * right,
            left.m44 * right,
        };
    }

    void Matrix4::multiply(float32 scalar)
    {
        *this = multiply(*this, scalar);
    }

    Matrix4 Matrix4::multiply(Matrix4 const& left, Matrix4 const& right)
    {
        return {
            left.m11 * right.m11 + left.m12 * right.m21 + left.m13 * right.m31 + left.m14 * right.m41,
            left.m11 * right.m12 + left.m12 * right.m22 + left.m13 * right.m32 + left.m14 * right.m42,
            left.m11 * right.m13 + left.m12 * right.m23 + left.m13 * right.m33 + left.m14 * right.m43,
            left.m11 * right.m14 + left.m12 * right.m24 + left.m13 * right.m34 + left.m14 * right.m44,
            left.m21 * right.m11 + left.m22 * right.m21 + left.m23 * right.m31 + left.m24 * right.m41,
            left.m21 * right.m12 + left.m22 * right.m22 + left.m23 * right.m32 + left.m24 * right.m42,
            left.m21 * right.m13 + left.m22 * right.m23 + left.m23 * right.m33 + left.m24 * right.m43,
            left.m21 * right.m14 + left.m22 * right.m24 + left.m23 * right.m34 + left.m24 * right.m44,
            left.m31 * right.m11 + left.m32 * right.m21 + left.m33 * right.m31 + left.m34 * right.m41,
            left.m31 * right.m12 + left.m32 * right.m22 + left.m33 * right.m32 + left.m34 * right.m42,
            left.m31 * right.m13 + left.m32 * right.m23 + left.m33 * right.m33 + left.m34 * right.m43,
            left.m31 * right.m14 + left.m32 * right.m24 + left.m33 * right.m34 + left.m34 * right.m44,
            left.m41 * right.m11 + left.m42 * right.m21 + left.m43 * right.m31 + left.m44 * right.m41,
            left.m41 * right.m12 + left.m42 * right.m22 + left.m43 * right.m32 + left.m44 * right.m42,
            left.m41 * right.m13 + left.m42 * right.m23 + left.m43 * right.m33 + left.m44 * right.m43,
            left.m41 * right.m14 + left.m42 * right.m24 + left.m43 * right.m34 + left.m44 * right.m44,
        };
    }

    void Matrix4::multiply(Matrix4 const& other)
    {
        *this = multiply(*this, other);
    }

    Vector4 Matrix4::transformPoint(Vector3 const& point, Matrix4 const& transform)
    {
        return {
            point.x * transform.m[0][0] + point.y * transform.m[1][0] + point.z * transform.m[2][0] + transform.m[3][0],
            point.x * transform.m[0][1] + point.y * transform.m[1][1] + point.z * transform.m[2][1] + transform.m[3][1],
            point.x * transform.m[0][2] + point.y * transform.m[1][2] + point.z * transform.m[2][2] + transform.m[3][2],
            point.x * transform.m[0][3] + point.y * transform.m[1][3] + point.z * transform.m[2][3] + transform.m[3][3],
        };
    }

    Vector4 Matrix4::transformPoint(Vector3 const& point) const
    {
        return transformPoint(point, *this);
    }

    Vector3 Matrix4::transformVector(Vector3 const& vector, Matrix4 const& transform)
    {
        float32 x = vector.x * transform.m[0][0] + vector.y * transform.m[1][0] + vector.z * transform.m[2][0];
        float32 y = vector.x * transform.m[0][1] + vector.y * transform.m[1][1] + vector.z * transform.m[2][1];
        float32 z = vector.x * transform.m[0][2] + vector.y * transform.m[1][2] + vector.z * transform.m[2][2];

        return {x, y, z};
    }

    Vector3 Matrix4::transformVector(Vector3 const& vector) const
    {
        return transformVector(vector, *this);
    }

    Vector4 Matrix4::transformVector(Vector4 const& vector, Matrix4 const& transform)
    {
        return {
            vector.x * transform.m[0][0] + vector.y * transform.m[1][0] + vector.z * transform.m[2][0] + vector.w * transform.m[3][0],
            vector.x * transform.m[0][1] + vector.y * transform.m[1][1] + vector.z * transform.m[2][1] + vector.w * transform.m[3][1],
            vector.x * transform.m[0][2] + vector.y * transform.m[1][2] + vector.z * transform.m[2][2] + vector.w * transform.m[3][2],
            vector.x * transform.m[0][3] + vector.y * transform.m[1][3] + vector.z * transform.m[2][3] + vector.w * transform.m[3][3],
        };
    }

    Vector4 Matrix4::transformVector(Vector4 const& vector) const
    {
        return transformVector(vector, *this);
    }

    bool Matrix4::isNearEqual(Matrix4 const& other) const
    {
        return Math::isNearEqual(m11, other.m11) &&
               Math::isNearEqual(m12, other.m12) &&
               Math::isNearEqual(m13, other.m13) &&
               Math::isNearEqual(m14, other.m14) &&
               Math::isNearEqual(m21, other.m21) &&
               Math::isNearEqual(m22, other.m22) &&
               Math::isNearEqual(m23, other.m23) &&
               Math::isNearEqual(m24, other.m24) &&
               Math::isNearEqual(m31, other.m31) &&
               Math::isNearEqual(m32, other.m32) &&
               Math::isNearEqual(m33, other.m33) &&
               Math::isNearEqual(m34, other.m34) &&
               Math::isNearEqual(m41, other.m41) &&
               Math::isNearEqual(m42, other.m42) &&
               Math::isNearEqual(m43, other.m43) &&
               Math::isNearEqual(m44, other.m44);
    }

    bool Matrix4::operator==(Matrix4 const& other) const
    {
        return m11 == other.m11 &&
               m12 == other.m12 &&
               m13 == other.m13 &&
               m14 == other.m14 &&
               m21 == other.m21 &&
               m22 == other.m22 &&
               m23 == other.m23 &&
               m24 == other.m24 &&
               m31 == other.m31 &&
               m32 == other.m32 &&
               m33 == other.m33 &&
               m34 == other.m34 &&
               m41 == other.m41 &&
               m42 == other.m42 &&
               m43 == other.m43 &&
               m44 == other.m44;
    }

    Matrix4 Matrix4::operator*(float32 scalar) const
    {
        return multiply(*this, scalar);
    }

    Matrix4 Matrix4::operator*(Matrix4 const& other) const
    {
        return multiply(*this, other);
    }
}
