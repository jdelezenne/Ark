#include "Ark/Math/Polygon.hpp"
#include "Ark/Math/BoundingRect.hpp"
#include "Ark/Math/Circle.hpp"

namespace Ark::Math
{
    usize Polygon::getPointCount() const
    {
        return points.getCount();
    }

    Collections::Slice<Vector2 const> Polygon::getPoints() const
    {
        return points.asSlice();
    }

    void Polygon::setPoints(Collections::Slice<Vector2> const& points)
    {
        this->points = points;
    }

    void Polygon::clear()
    {
        points.clear();
    }

    void Polygon::resize(usize pointCount)
    {
        points.resize(pointCount);
    }

    void Polygon::append(Vector2 const& point)
    {
        points.append(point);
    }

    void Polygon::invert()
    {
        reverse(points.getStartIterator(), points.getEndIterator());
    }

    Vector2 Polygon::getCentre() const
    {
        return getBoundingCircle().center;
    }

    BoundingRect Polygon::getBoundingRect() const
    {
        if (points.isEmpty())
        {
            return {};
        }

        float32 x1 = NumericLimits<float32>::max();
        float32 y1 = NumericLimits<float32>::max();
        float32 x2 = NumericLimits<float32>::lowest();
        float32 y2 = NumericLimits<float32>::lowest();

        for (auto const& point : points)
        {
            x1 = Ark::min(x1, point.x);
            x2 = Ark::max(x2, point.x);
            y1 = Ark::min(y1, point.y);
            y2 = Ark::max(y2, point.y);
        }

        return {{x1, y1}, {x2, y2}};
    }

    Circle Polygon::getBoundingCircle() const
    {
        if (points.isEmpty())
        {
            return {};
        }

        Vector2 center{Vector2::Zero};
        for (auto const& point : points)
        {
            center += point;
        }
        center /= float32(points.getCount());

        float32 radius2{0};
        for (auto const& point : points)
        {
            float32 const distance = (point - center).getLengthSquared();
            if (distance > radius2)
            {
                radius2 = distance;
            }
        }

        return {center, Math::sqrt(radius2)};
    }

    void Polygon::translate(Vector2 const& offset)
    {
        for (auto& point : points)
        {
            point.translate(offset);
        }
    }

    void Polygon::rotate(float32 angle)
    {
        for (auto& point : points)
        {
            point.rotate(angle);
        }
    }

    void Polygon::scale(Vector2 const& scale)
    {
        for (auto& point : points)
        {
            point.scale(scale);
        }
    }

    Vector2 const& Polygon::operator[](usize index) const
    {
        ARK_ASSERT_MSG(index < points.getCount(), "Index is out of range");

        return points[index];
    }

    Vector2& Polygon::operator[](usize index)
    {
        ARK_ASSERT_MSG(index < points.getCount(), "Index is out of range");

        return points[index];
    }
}
