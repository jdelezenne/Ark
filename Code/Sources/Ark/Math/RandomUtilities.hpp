#pragma once

#include "Ark/Math/Random.hpp"
#include "Ark/Math/Types.hpp"

namespace Ark::Math
{
    /// Generates a random unit vector (length = 1).
    /// @param rng The random number generator.
    /// @return A random unit Vector2.
    Vector2 randomVector2(Random& rng);

    /// Generates a random Vector2 within specified bounds.
    /// @param rng The random number generator.
    /// @param min The minimum bounds.
    /// @param max The maximum bounds.
    /// @return A random Vector2 within the specified range.
    Vector2 randomVector2(Random& rng, Vector2 const& min, Vector2 const& max);

    /// Generates a random point inside a circle.
    /// @param rng The random number generator.
    /// @param radius The radius of the circle.
    /// @return A random point inside the circle.
    Vector2 randomPointInCircle(Random& rng, float32 radius);

    /// Generates a random point on the circumference of a circle.
    /// @param rng The random number generator.
    /// @param radius The radius of the circle.
    /// @return A random point on the circle.
    Vector2 randomPointOnCircle(Random& rng, float32 radius);

    /// Generates a random unit vector (length = 1).
    /// @param rng The random number generator.
    /// @return A random unit Vector3.
    Vector3 randomVector3(Random& rng);

    /// Generates a random Vector3 within specified bounds.
    /// @param rng The random number generator.
    /// @param min The minimum bounds.
    /// @param max The maximum bounds.
    /// @return A random Vector3 within the specified range.
    Vector3 randomVector3(Random& rng, Vector3 const& min, Vector3 const& max);

    /// Generates a random point inside a sphere.
    /// @param rng The random number generator.
    /// @param radius The radius of the sphere.
    /// @return A random point inside the sphere.
    Vector3 randomPointInSphere(Random& rng, float32 radius);

    /// Generates a random point on the surface of a sphere.
    /// @param rng The random number generator.
    /// @param radius The radius of the sphere.
    /// @return A random point on the sphere.
    Vector3 randomPointOnSphere(Random& rng, float32 radius);

    /// Generates a random rotation quaternion.
    /// @param rng The random number generator.
    /// @return A random unit quaternion representing a rotation.
    Quaternion randomRotation(Random& rng);

    /// Generates a random angle in radians [0, 2π).
    /// @param rng The random number generator.
    /// @return A random angle in radians.
    float32 randomAngle(Random& rng);

    /// Returns true with the specified probability.
    /// @param rng The random number generator.
    /// @param probability The probability value [0.0, 1.0].
    /// @return True if the random value is less than the probability.
    inline bool randomChance(Random& rng, float32 probability)
    {
        return rng.nextFloat32() < probability;
    }
}
