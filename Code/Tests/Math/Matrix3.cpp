#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Matrix3.hpp"
#include "Ark/Math/Vector2.hpp"

ARK_TEST_CASE("Matrix3", "[matrix3]")
{
    using Ark::Math::Matrix3;

    SECTION("identity and equality")
    {
        Matrix3 identity = Matrix3::Identity;
        REQUIRE(identity == Matrix3::Identity);
        REQUIRE(identity.isIdentity());
    }

    SECTION("translation and transformPoint")
    {
        using Ark::Math::Vector2;

        Matrix3 t = Matrix3::translation({3.0f, 4.0f});
        Vector2 p{1.0f, 2.0f};
        Vector2 r = Matrix3::transformPoint(p, t);

        REQUIRE(r.x == Catch::Approx(4.0f));
        REQUIRE(r.y == Catch::Approx(6.0f));
    }

    SECTION("scaling and base scale")
    {
        using Ark::Math::Vector2;

        Matrix3 s = Matrix3::scaling({2.0f, 3.0f});
        REQUIRE(s.getScale().x == Catch::Approx(2.0f));
        REQUIRE(s.getScale().y == Catch::Approx(3.0f));

        s.setBaseRotation(0.0f);
        Vector2 scale = s.getBaseScale();
        REQUIRE(scale.x == Catch::Approx(2.0f));
        REQUIRE(scale.y == Catch::Approx(3.0f));
    }

    SECTION("rotation inverse equals identity")
    {
        using Ark::Math::degreesToRadians;

        float const angle = degreesToRadians(90.0f);
        Matrix3 rot = Matrix3::rotation(angle);
        Matrix3 inv = Matrix3::rotation(-angle);

        Matrix3 m = rot * inv;
        REQUIRE(m.isNearEqual(Matrix3::Identity));
    }

    SECTION("rotation transforms unit axis")
    {
        using Ark::Math::degreesToRadians;
        using Ark::Math::Vector2;

        float const angle = degreesToRadians(90.0f);
        Matrix3 rot = Matrix3::rotation(angle);

        Vector2 x{1.0f, 0.0f};
        Vector2 y = Matrix3::transformVector(x, rot);

        auto approx = [](float v)
        {
            return Catch::Approx(v).margin(1e-5f);
        };
        REQUIRE(y.x == approx(0.0f));
        REQUIRE(y.y == approx(1.0f));
    }
}
