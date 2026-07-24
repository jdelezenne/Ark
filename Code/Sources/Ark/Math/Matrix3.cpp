#include "Ark/Math/Matrix3.hpp"
#include "Ark/Math/Matrix4.hpp"
#include "Ark/Core/Functions.hpp"

namespace Ark::Math
{
    // clang-format off
    const Matrix3 Matrix3::Zero = Matrix3(
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f);

    const Matrix3 Matrix3::Identity = Matrix3(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f);
    // clang-format on

    Matrix3::Matrix3(Matrix4 const& matrix)
    {
        Memory::copy(&matrix.m11, &m11, sizeof(Vector3));
        Memory::copy(&matrix.m21, &m21, sizeof(Vector3));
        Memory::copy(&matrix.m31, &m31, sizeof(Vector3));
    }

    float32 Matrix3::getDeterminant() const
    {
        return m11 * m22 * m33 +
               m12 * m23 * m31 +
               m13 * m21 * m32 -
               m13 * m22 * m31 -
               m12 * m21 * m33 -
               m11 * m23 * m32;
    }

    void Matrix3::transpose()
    {
        Ark::swap(m12, m21);
        Ark::swap(m13, m31);
        Ark::swap(m23, m32);
    }

    Matrix3 Matrix3::getTransposed() const
    {
        Matrix3 result = *this;
        result.transpose();
        return result;
    }

    Outcome Matrix3::inverse()
    {
        float32 const determinant = getDeterminant();

        if (Math::isZero(determinant))
        {
            return makeError();
        }

        float32 const inverseDeterminant = 1.0f / determinant;

        Matrix3 result;
        result.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * inverseDeterminant;
        result.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * inverseDeterminant;
        result.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * inverseDeterminant;
        result.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * inverseDeterminant;
        result.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * inverseDeterminant;
        result.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * inverseDeterminant;
        result.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * inverseDeterminant;
        result.m[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * inverseDeterminant;
        result.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * inverseDeterminant;
        *this = result;

        return makeOutcome();
    }

    Matrix3 Matrix3::getInversed() const
    {
        Matrix3 result = *this;
        result.inverse();
        return result;
    }

    Matrix3 Matrix3::translation(Vector2 const& translation)
    {
        return {
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {translation.x, translation.y, 1.0f},
        };
    }

    Vector2 Matrix3::getTranslation() const
    {
        return {m31, m32};
    }

    void Matrix3::setTranslation(Vector2 const& translation)
    {
        m31 = translation.x;
        m32 = translation.y;
    }

    void Matrix3::translate(Vector2 const& position)
    {
        m31 += position.x;
        m32 += position.y;
    }

    Matrix3 Matrix3::rotation(float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        return {
            {cosine, sine, 0.0f},
            {-sine, cosine, 0.0f},
            {0.0f, 0.0f, 1.0f},
        };
    }

    void Matrix3::setRotation(float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        m11 = cosine;
        m12 = sine;
        m21 = -sine;
        m22 = cosine;
    }

    void Matrix3::rotate(float32 angle)
    {
        *this *= rotation(angle);
    }

    Matrix3 Matrix3::scaling(Vector2 const& scale)
    {
        return {
            {scale.x, 0.0f, 0.0f},
            {0.0f, scale.y, 0.0f},
            {0.0f, 0.0f, 1.0f},
        };
    }

    Vector2 Matrix3::getScale() const
    {
        return {m11, m22};
    }

    void Matrix3::setScale(Vector2 const& scale)
    {
        m11 = scale.x;
        m22 = scale.y;
    }

    void Matrix3::scale(Vector2 const& scale)
    {
        *this *= scaling(scale);
    }

    float32 Matrix3::getBaseRotation() const
    {
        return Math::arcTan2(m[0][1], m[0][0]);
    }

    void Matrix3::setBaseRotation(float32 angle)
    {
        Vector2 const scale = getBaseScale();

        float32 cosine = Math::cos(angle);
        float32 sine = Math::sin(angle);

        m11 = cosine;
        m12 = sine;
        m21 = -sine;
        m22 = cosine;

        setBaseScale(scale);
    }

    Vector2 Matrix3::getBaseScale() const
    {
        float32 sign = static_cast<float32>(Math::sign(getDeterminant()));
        Vector3 x = getColumn(0);
        Vector3 y = getColumn(1);

        return {x.getLength(), sign * y.getLength()};
    }

    void Matrix3::setBaseScale(Vector2 const& scale)
    {
        Vector3 x = getRow(0).getNormalized();
        Vector3 y = getRow(1).getNormalized();

        setRow(0, x * scale.x);
        setRow(1, y * scale.y);
    }

    Matrix3 Matrix3::orthographic(float32 width, float32 height)
    {
        float32 const halfWidth = width * 0.5f;
        float32 const halfHeight = height * 0.5f;

        return orthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight);
    }

    Matrix3 Matrix3::orthographicOffCenter(float32 left, float32 right, float32 bottom, float32 top)
    {
        Matrix3 result = Matrix3::Identity;
        result.m11 = 2.0f / (right - left);
        result.m22 = 2.0f / (top - bottom);
        result.m31 = (left + right) / (left - right);
        result.m32 = (top + bottom) / (bottom - top);
        return result;
    }

    Matrix3 Matrix3::multiply(Matrix3 const& left, float32 right)
    {
        return {
            left.m11 * right,
            left.m12 * right,
            left.m13 * right,
            left.m21 * right,
            left.m22 * right,
            left.m23 * right,
            left.m31 * right,
            left.m32 * right,
            left.m33 * right,
        };
    }

    void Matrix3::multiply(float32 scalar)
    {
        *this = multiply(*this, scalar);
    }

    Matrix3 Matrix3::multiply(Matrix3 const& left, Matrix3 const& right)
    {
        return {
            left.m11 * right.m11 + left.m12 * right.m21 + left.m13 * right.m31,
            left.m11 * right.m12 + left.m12 * right.m22 + left.m13 * right.m32,
            left.m11 * right.m13 + left.m12 * right.m23 + left.m13 * right.m33,
            left.m21 * right.m11 + left.m22 * right.m21 + left.m23 * right.m31,
            left.m21 * right.m12 + left.m22 * right.m22 + left.m23 * right.m32,
            left.m21 * right.m13 + left.m22 * right.m23 + left.m23 * right.m33,
            left.m31 * right.m11 + left.m32 * right.m21 + left.m33 * right.m31,
            left.m31 * right.m12 + left.m32 * right.m22 + left.m33 * right.m32,
            left.m31 * right.m13 + left.m32 * right.m23 + left.m33 * right.m33,
        };
    }

    void Matrix3::multiply(Matrix3 const& other)
    {
        *this = multiply(*this, other);
    }

    Vector2 Matrix3::transformPoint(Vector2 const& point, Matrix3 const& transform)
    {
        return {
            point.x * transform.m11 + point.y * transform.m21 + transform.m31,
            point.x * transform.m12 + point.y * transform.m22 + transform.m32};
    }

    Vector2 Matrix3::transformPoint(Vector2 const& point) const
    {
        return transformPoint(point, *this);
    }

    Vector2 Matrix3::transformVector(Vector2 const& vector, Matrix3 const& transform)
    {
        return {
            vector.x * transform.m11 + vector.y * transform.m21,
            vector.x * transform.m12 + vector.y * transform.m22};
    }

    Vector2 Matrix3::transformVector(Vector2 const& vector) const
    {
        return transformVector(vector, *this);
    }

    bool Matrix3::isNearEqual(Matrix3 const& other) const
    {
        return Math::isNearEqual(m11, other.m11) &&
               Math::isNearEqual(m12, other.m12) &&
               Math::isNearEqual(m13, other.m13) &&
               Math::isNearEqual(m21, other.m21) &&
               Math::isNearEqual(m22, other.m22) &&
               Math::isNearEqual(m23, other.m23) &&
               Math::isNearEqual(m31, other.m31) &&
               Math::isNearEqual(m32, other.m32) &&
               Math::isNearEqual(m33, other.m33);
    }

    bool Matrix3::operator==(Matrix3 const& other) const
    {
        return m11 == other.m11 &&
               m12 == other.m12 &&
               m13 == other.m13 &&
               m21 == other.m21 &&
               m22 == other.m22 &&
               m23 == other.m23 &&
               m31 == other.m31 &&
               m32 == other.m32 &&
               m33 == other.m33;
    }

    Matrix3 Matrix3::operator*(Matrix3 const& m) const
    {
        return {m.m11 * m11 + m.m21 * m12 + m.m31 * m13,
                m.m12 * m11 + m.m22 * m12 + m.m32 * m13,
                m.m13 * m11 + m.m23 * m12 + m.m33 * m13,
                m.m11 * m21 + m.m21 * m22 + m.m31 * m23,
                m.m12 * m21 + m.m22 * m22 + m.m32 * m23,
                m.m13 * m21 + m.m23 * m22 + m.m33 * m23,
                m.m11 * m31 + m.m21 * m32 + m.m31 * m33,
                m.m12 * m31 + m.m22 * m32 + m.m32 * m33,
                m.m13 * m31 + m.m23 * m32 + m.m33 * m33};
    }
}
