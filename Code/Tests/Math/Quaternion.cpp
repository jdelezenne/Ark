#include <catch2/catch_approx.hpp>

#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Types.hpp"
#include "Ark/Math/Quaternion.hpp"
#include "Ark/Math/Vector3.hpp"

using Ark::float32;
using Ark::Math::Quaternion;
using Ark::Math::Vector3;

static auto approx(float v)
{
    return Catch::Approx(v).margin(1e-5f);
}

ARK_TEST_CASE("Quaternion", "[math][quaternion]")
{
    SECTION("from Euler and vector transform")
    {
        Quaternion q = Quaternion::fromEulerAngles(0.0f, Ark::Math::degreesToRadians(90.0f), 0.0f);
        Vector3 v = q.transformVector({1.0f, 0.0f, 0.0f});

        REQUIRE(v.x == approx(0.0f));
        REQUIRE(v.y == approx(0.0f));
        REQUIRE(v.z == approx(-1.0f));
    }

    SECTION("conjugate")
    {
        Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
        Quaternion conj = q.getConjugate();
        REQUIRE(conj.x == -1.0f);
        REQUIRE(conj.y == -2.0f);
        REQUIRE(conj.z == -3.0f);
        REQUIRE(conj.w == 4.0f);
    }

    SECTION("dot product")
    {
        Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
        Quaternion b(5.0f, 6.0f, 7.0f, 8.0f);
        float32 d = Quaternion::dot(a, b);
        REQUIRE(d == approx(70.0f));
    }

    SECTION("angle between")
    {
        Quaternion a = Quaternion::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
        Quaternion b = Quaternion::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), Ark::Math::degreesToRadians(90.0f));
        float32 angle = Quaternion::angle(a, b);
        REQUIRE(angle == approx(Ark::Math::degreesToRadians(90.0f)));
    }

    SECTION("lookRotation")
    {
        Vector3 forward(0.0f, 0.0f, 1.0f);
        Vector3 up(0.0f, 1.0f, 0.0f);
        Quaternion q = Quaternion::lookRotation(forward, up);
        Vector3 transformed = q.transformVector(Vector3(0.0f, 0.0f, 1.0f));
        REQUIRE(transformed.x == approx(0.0f));
        REQUIRE(transformed.y == approx(0.0f));
        REQUIRE(transformed.z == approx(1.0f));
    }

    SECTION("slerp midpoint")
    {
        Quaternion start = Quaternion::Identity;
        Quaternion end = Quaternion::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), Ark::Math::degreesToRadians(90.0f));
        Quaternion mid = Quaternion::slerp(start, end, 0.5f);
        Vector3 v = mid.transformVector(Vector3(1.0f, 0.0f, 0.0f));
        REQUIRE(v.x == approx(0.70710678f));
        REQUIRE(v.y == approx(0.0f));
        REQUIRE(v.z == approx(-0.70710678f));
    }

    SECTION("slerp endpoints")
    {
        Quaternion start = Quaternion::Identity;
        Quaternion end = Quaternion::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), Ark::Math::degreesToRadians(90.0f));

        Quaternion atStart = Quaternion::slerp(start, end, 0.0f);
        Quaternion atEnd = Quaternion::slerp(start, end, 1.0f);
        REQUIRE(atStart.x == approx(start.x));
        REQUIRE(atStart.y == approx(start.y));
        REQUIRE(atStart.z == approx(start.z));
        REQUIRE(atStart.w == approx(start.w));
        REQUIRE(atEnd.x == approx(end.x));
        REQUIRE(atEnd.y == approx(end.y));
        REQUIRE(atEnd.z == approx(end.z));
        REQUIRE(atEnd.w == approx(end.w));
    }

    SECTION("lerp shortest path")
    {
        Quaternion start = Quaternion::Identity;
        Quaternion end = Quaternion(-0.0f, -0.0f, -0.0f, -1.0f);
        Quaternion mid = Quaternion::lerp(start, end, 0.5f);
        REQUIRE(mid.getLengthSquared() == approx(1.0f));
    }

    SECTION("division")
    {
        Quaternion a = Quaternion::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), Ark::Math::degreesToRadians(90.0f));
        Quaternion b = Quaternion::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), Ark::Math::degreesToRadians(45.0f));
        Quaternion result = a / b;
        Quaternion expected = Quaternion::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), Ark::Math::degreesToRadians(45.0f));
        REQUIRE(result.x == approx(expected.x));
        REQUIRE(result.y == approx(expected.y));
        REQUIRE(result.z == approx(expected.z));
        REQUIRE(result.w == approx(expected.w));
    }

    SECTION("multiply identity")
    {
        Quaternion q = Quaternion::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), Ark::Math::degreesToRadians(45.0f));
        Quaternion result = q * Quaternion::Identity;
        REQUIRE(result.x == approx(q.x));
        REQUIRE(result.y == approx(q.y));
        REQUIRE(result.z == approx(q.z));
        REQUIRE(result.w == approx(q.w));
    }

    SECTION("normalize preserves direction")
    {
        Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
        Quaternion normalized = q.getNormalized();
        REQUIRE(normalized.getLengthSquared() == approx(1.0f));

        float32 ratio = normalized.x / q.x;
        REQUIRE(normalized.y / q.y == approx(ratio));
        REQUIRE(normalized.z / q.z == approx(ratio));
        REQUIRE(normalized.w / q.w == approx(ratio));
    }
}
