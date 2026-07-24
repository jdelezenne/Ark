#include "Ark/Math/RandomUtilities.hpp"

#include "Ark/Math/Constants.hpp"
#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Intrinsics.hpp"
#include "Ark/Math/Quaternion.hpp"
#include "Ark/Math/Vector2.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    Vector2 randomVector2(Random& rng)
    {
        float32 angle = rng.nextFloat32(0.0f, TwoPi);
        return Vector2(Math::cos(angle), Math::sin(angle));
    }

    Vector2 randomVector2(Random& rng, Vector2 const& min, Vector2 const& max)
    {
        float32 x = rng.nextFloat32(min.x, max.x);
        float32 y = rng.nextFloat32(min.y, max.y);
        return Vector2(x, y);
    }

    Vector2 randomPointInCircle(Random& rng, float32 radius)
    {
        float32 angle = rng.nextFloat32(0.0f, TwoPi);
        float32 r = radius * Math::sqrt(rng.nextFloat32());
        return Vector2(r * Math::cos(angle), r * Math::sin(angle));
    }

    Vector2 randomPointOnCircle(Random& rng, float32 radius)
    {
        float32 angle = rng.nextFloat32(0.0f, TwoPi);
        return Vector2(radius * Math::cos(angle), radius * Math::sin(angle));
    }

    Vector3 randomVector3(Random& rng)
    {
        float32 z = rng.nextFloat32(-1.0f, 1.0f);
        float32 angle = rng.nextFloat32(0.0f, TwoPi);
        float32 r = Math::sqrt(1.0f - z * z);
        return Vector3(r * Math::cos(angle), r * Math::sin(angle), z);
    }

    Vector3 randomVector3(Random& rng, Vector3 const& min, Vector3 const& max)
    {
        float32 x = rng.nextFloat32(min.x, max.x);
        float32 y = rng.nextFloat32(min.y, max.y);
        float32 z = rng.nextFloat32(min.z, max.z);
        return Vector3(x, y, z);
    }

    Vector3 randomPointInSphere(Random& rng, float32 radius)
    {
        float32 u = rng.nextFloat32();
        float32 v = rng.nextFloat32();
        float32 theta = u * TwoPi;
        float32 phi = Math::arcCos(2.0f * v - 1.0f);
        float32 r = radius * Math::cbrt(rng.nextFloat32());
        float32 sinPhi = Math::sin(phi);
        return Vector3(r * sinPhi * Math::cos(theta), r * sinPhi * Math::sin(theta), r * Math::cos(phi));
    }

    Vector3 randomPointOnSphere(Random& rng, float32 radius)
    {
        float32 u = rng.nextFloat32();
        float32 v = rng.nextFloat32();
        float32 theta = u * TwoPi;
        float32 phi = Math::arcCos(2.0f * v - 1.0f);
        float32 sinPhi = Math::sin(phi);
        return Vector3(radius * sinPhi * Math::cos(theta), radius * sinPhi * Math::sin(theta), radius * Math::cos(phi));
    }

    Quaternion randomRotation(Random& rng)
    {
        float32 u1 = rng.nextFloat32();
        float32 u2 = rng.nextFloat32();
        float32 u3 = rng.nextFloat32();

        float32 sqrt1MinusU1 = Math::sqrt(1.0f - u1);
        float32 sqrtU1 = Math::sqrt(u1);

        return Quaternion(
            sqrt1MinusU1 * Math::sin(TwoPi * u2),
            sqrt1MinusU1 * Math::cos(TwoPi * u2),
            sqrtU1 * Math::sin(TwoPi * u3),
            sqrtU1 * Math::cos(TwoPi * u3));
    }

    float32 randomAngle(Random& rng)
    {
        return rng.nextFloat32(0.0f, TwoPi);
    }
}
