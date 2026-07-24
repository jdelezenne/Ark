#include "Ark/Math/Vector2.hpp"
#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Matrix3.hpp"
#include "Ark/Math/Point2.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Math
{
    Vector2 const Vector2::Zero = Vector2(0.0f, 0.0f);
    Vector2 const Vector2::One = Vector2(1.0f, 1.0f);
    Vector2 const Vector2::UnitX = Vector2(1.0f, 0.0f);
    Vector2 const Vector2::UnitY = Vector2(0.0f, 1.0f);

    Vector2::Vector2(float32 const values[2])
    {
        Memory::copy(values, this->values, sizeof(this->values));
    }

    Vector2::Vector2(Point2Float const& point)
        : x{point.x}
        , y{point.y}
    {
    }

    float32 Vector2::getLength() const
    {
        return Math::sqrt(x * x + y * y);
    }

    float32 Vector2::getLengthSquared() const
    {
        return (x * x + y * y);
    }

    float32 Vector2::getDistance(Vector2 const& a, Vector2 const& b)
    {
        return (a - b).getLength();
    }

    float32 Vector2::getDistanceSquared(Vector2 const& a, Vector2 const& b)
    {
        float32 const x = (a.x - b.x);
        float32 const y = (a.y - b.y);

        return (x * x + y * y);
    }

    float32 Vector2::getAngle(Vector2 const& a, Vector2 const& b)
    {
        float32 const dotProduct = (a.x * b.x + a.y * b.y);
        float32 const determinant = (a.x * b.y - a.y * b.x);

        return Math::arcTan2(determinant, dotProduct);
    }

    void Vector2::normalize()
    {
        float32 const length = getLength();

        if (length >= Math::ZeroTolerance<float32>)
        {
            float32 const inverse = 1.0f / length;
            x *= inverse;
            y *= inverse;
        }
    }

    Vector2 Vector2::getNormalized() const
    {
        Vector2 result = *this;
        result.normalize();
        return result;
    }

    Vector2 Vector2::perpendicular() const
    {
        return {-y, x};
    }

    float32 Vector2::dot(Vector2 const& a, Vector2 const& b)
    {
        return (a.x * b.x) + (a.y * b.y);
    }

    float32 Vector2::dot(Vector2 const& other)
    {
        return Vector2::dot(*this, other);
    }

    float32 Vector2::cross(Vector2 const& a, Vector2 const& b)
    {
        return (a.x * b.y - a.y * b.x);
    }

    float32 Vector2::cross(Vector2 const& other)
    {
        return Vector2::cross(*this, other);
    }

    Vector2 Vector2::translated(Vector2 const& translation) const
    {
        return (*this + translation);
    }

    void Vector2::translate(Vector2 const& translation)
    {
        *this += translation;
    }

    Vector2 Vector2::rotated(float32 angle) const
    {
        float32 const sine = Math::sin(angle);
        float32 const cosine = Math::cos(angle);

        return {
            x * cosine - y * sine,
            x * sine + y * cosine,
        };
    }

    void Vector2::rotate(float32 angle)
    {
        *this = rotated(angle);
    }

    Vector2 Vector2::scaled(Vector2 const& scale) const
    {
        return (*this * scale);
    }

    void Vector2::scale(Vector2 const& scale)
    {
        *this *= scale;
    }

    Vector2 Vector2::min(Vector2 const& a, Vector2 const& b)
    {
        return {
            Math::min(a.x, b.x),
            Math::min(a.y, b.y),
        };
    }

    Vector2 Vector2::max(Vector2 const& a, Vector2 const& b)
    {
        return {
            Math::max(a.x, b.x),
            Math::max(a.y, b.y),
        };
    }

    Vector2 Vector2::clamp(Vector2 const& vector, Vector2 const& min, Vector2 const& max)
    {
        return {
            Math::clamp(vector.x, min.x, max.x),
            Math::clamp(vector.y, min.y, max.y),
        };
    }

    Vector2 Vector2::lerp(Vector2 const& start, Vector2 const& end, float32 amount)
    {
        return {
            Math::lerp(start.x, end.x, amount),
            Math::lerp(start.y, end.y, amount),
        };
    }

    Vector2 Vector2::smoothStep(Vector2 const& start, Vector2 const& end, float32 amount)
    {
        amount = Math::smoothStep(amount);
        return {
            Math::lerp(start.x, end.x, amount),
            Math::lerp(start.y, end.y, amount),
        };
    }

    Vector2 Vector2::midpoint(Vector2 const& p1, Vector2 const& p2)
    {
        return {(p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f};
    }

    Vector2 Vector2::normal(Vector2 const& p1, Vector2 const& p2)
    {
        Vector2 const normal = (p2 - p1).perpendicular();
        return normal.getNormalized();
    }

    Vector2 Vector2::transform(Vector2 const& vector, Matrix3 const& transform)
    {
        return {
            vector.x * transform.m11 + vector.y * transform.m21,
            vector.x * transform.m12 + vector.y * transform.m22,
        };
    }

    Vector2 Vector2::project(Vector2 const& vector, Vector2 const& normal)
    {
        float32 const squaredLength = dot(normal, normal);

        if (squaredLength < Math::ZeroTolerance<float32>)
        {
            return Zero;
        }

        return normal * dot(vector, normal) / squaredLength;
    }

    Vector2 Vector2::reflect(Vector2 const& vector, Vector2 const& normal)
    {
        float32 const squaredLength = dot(normal, normal);

        if (squaredLength < Math::ZeroTolerance<float32>)
        {
            return Zero;
        }

        Vector2 const projection = project(vector, normal);
        return vector - (projection * 2.0f);
    }

    Vector2 Vector2::rotate(Vector2 const& vector, float32 angle)
    {
        float32 const cosine = Math::cos(angle);
        float32 const sine = Math::sin(angle);

        return {
            vector.x - (vector.x * cosine - vector.y * sine),
            vector.y - (vector.x * sine + vector.y * cosine),
        };
    }

    Vector2 Vector2::moveTowards(Vector2 const& vector, Vector2 const& target, float32 maxDistance)
    {
        float32 const deltaX = (target.x - vector.x);
        float32 const deltaY = (target.y - vector.y);
        float32 const value = (deltaX * deltaX) + (deltaY * deltaY);

        if (Math::isZero(value))
        {
            return target;
        }

        if ((maxDistance >= 0) && (value <= maxDistance * maxDistance))
        {
            return target;
        }

        float32 const distance = Math::sqrt(value);

        return {
            vector.x + deltaX / distance * maxDistance,
            vector.y + deltaY / distance * maxDistance,
        };
    }

    Vector2 Vector2::invert(Vector2 const& vector)
    {
        return {
            1.0f / vector.x,
            1.0f / vector.y,
        };
    }

    Vector2 Vector2::negate(Vector2 const& vector)
    {
        return -vector;
    }

    Vector2 Vector2::add(Vector2 const& a, Vector2 const& b)
    {
        return (a + b);
    }

    Vector2 Vector2::add(Vector2 const& vector, float32 scalar)
    {
        return (vector + scalar);
    }

    Vector2 Vector2::subtract(Vector2 const& a, Vector2 const& b)
    {
        return (a - b);
    }

    Vector2 Vector2::subtract(Vector2 const& vector, float32 scalar)
    {
        return (vector - scalar);
    }

    Vector2 Vector2::multiply(Vector2 const& a, Vector2 const& b)
    {
        return (a * b);
    }

    Vector2 Vector2::multiply(Vector2 const& vector, float32 scalar)
    {
        return (vector * scalar);
    }

    Vector2 Vector2::divide(Vector2 const& a, Vector2 const& b)
    {
        return (a / b);
    }

    Vector2 Vector2::divide(Vector2 const& vector, float32 scalar)
    {
        return (vector / scalar);
    }
}
