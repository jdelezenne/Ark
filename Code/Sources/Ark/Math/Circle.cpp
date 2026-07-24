#include "Ark/Math/Circle.hpp"
#include "Ark/Math/Rect.hpp"

namespace Ark::Math
{
    bool Circle::contains(Vector2 const& point) const
    {
        return (point - center).getLengthSquared() <= (radius * radius);
    }

    bool Circle::overlaps(Circle const& circle) const
    {
        float32 const radii = (radius + circle.radius);
        return (circle.center - center).getLengthSquared() <= (radii * radii);
    }

    Circle Circle::expand(float radius) const
    {
        return {center, this->radius + radius};
    }

    RectFloat Circle::getAABB() const
    {
        return RectFloat::fromCorners(center.x - radius,
                                      center.y - radius,
                                      center.x + radius,
                                      center.y + radius);
    }

    float32 Circle::getDistanceTo(Vector2 const& point) const
    {
        return Math::max((center - point).getLength() - radius, 0.0f);
    }

    float32 Circle::getDistanceTo(Circle const& circle) const
    {
        return Math::max((center - circle.center).getLength() - radius - circle.radius, 0.0f);
    }
}
