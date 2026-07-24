#include "Ark/Math/Sphere.hpp"
#include "Ark/Math/BoundingBox.hpp"

namespace Ark::Math
{
    bool Sphere::contains(Vector3 const& point) const
    {
        return (point - center).getLengthSquared() <= (radius * radius);
    }

    bool Sphere::overlaps(Sphere const& circle) const
    {
        float32 const radii = (radius + circle.radius);
        return (circle.center - center).getLengthSquared() <= (radii * radii);
    }

    Sphere Sphere::expand(float radius) const
    {
        return {center, this->radius + radius};
    }

    BoundingBox Sphere::getAABB() const
    {
        return {center - Vector3(radius), center + Vector3(radius)};
    }

    float32 Sphere::getDistanceTo(Vector3 const& point) const
    {
        return (center - point).getLength() - radius;
    }

    float32 Sphere::getDistanceTo(Sphere const& circle) const
    {
        return (center - circle.center).getLength() - radius - circle.radius;
    }
}
