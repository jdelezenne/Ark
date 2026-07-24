#include <catch2/catch_approx.hpp>

#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Types.hpp"
#include "Ark/Math/Matrix4.hpp"
#include "Ark/Math/Types.hpp"

ARK_TEST_CASE("Matrix4", "[math][matrix4]")
{
    SECTION("identity")
    {
        using Ark::Math::Matrix4;

        Matrix4 identity = Matrix4::Identity;
        REQUIRE(identity == Matrix4::Identity);
    }

    SECTION("translation and transformPoint")
    {
        using Ark::Math::Matrix4;
        using Ark::Math::Vector4;

        Matrix4 t = Matrix4::translation({2.0f, 3.0f, 4.0f});
        Vector4 p = Matrix4::transformPoint({1.0f, 2.0f, 3.0f}, t);
        REQUIRE(p.x == Catch::Approx(3.0f));
        REQUIRE(p.y == Catch::Approx(5.0f));
        REQUIRE(p.z == Catch::Approx(7.0f));
        REQUIRE(p.w == Catch::Approx(1.0f));
    }

    SECTION("rotationZ transforms unit axis")
    {
        using Ark::Math::CoordinateSystem;
        using Ark::Math::CurrentCoordinateSystem;
        using Ark::Math::degreesToRadians;
        using Ark::Math::Matrix4;
        using Ark::Math::Vector3;

        float const angle = degreesToRadians(90.0f);
        Matrix4 rot = Matrix4::rotationZ(angle);
        Vector3 x{1.0f, 0.0f, 0.0f};
        Vector3 y = Matrix4::transformVector(x, rot);
        auto approx = [](float v)
        {
            return Catch::Approx(v).margin(1e-5f);
        };
        REQUIRE(y.x == approx(0.0f));
        if constexpr (CurrentCoordinateSystem == CoordinateSystem::RightHanded)
        {
            REQUIRE(y.y == approx(-1.0f));
        }
        else
        {
            REQUIRE(y.y == approx(1.0f));
        }
        REQUIRE(y.z == approx(0.0f));
    }

    SECTION("perspective LH and RH m34 sign")
    {
        using Ark::float32;
        using Ark::Math::Matrix4;
        using Ark::Math::PiOver4;

        float32 const fov = PiOver4;
        float32 const aspect = 16.0f / 9.0f;
        float32 const nearPlane = 0.1f;
        float32 const farPlane = 100.0f;

        Matrix4 projLH = Matrix4::perspectiveFieldOfViewLH(fov, aspect, nearPlane, farPlane);
        Matrix4 projRH = Matrix4::perspectiveFieldOfViewRH(fov, aspect, nearPlane, farPlane);

        REQUIRE(projLH.m34 == Catch::Approx(1.0f));
        REQUIRE(projRH.m34 == Catch::Approx(-1.0f));
    }

    SECTION("lookAt LH and RH forward vectors")
    {
        using Ark::Math::Matrix4;
        using Ark::Math::Vector3;

        Vector3 const eye(0.0f, 0.0f, -5.0f);
        Vector3 const target(0.0f, 0.0f, 0.0f);
        Vector3 const up(0.0f, 1.0f, 0.0f);

        auto approx = [](float v)
        {
            return Catch::Approx(v).margin(0.01f);
        };

        Matrix4 viewLH = Matrix4::lookAtLH(eye, target, up);
        Vector3 forwardLH = viewLH.getForward();
        REQUIRE(forwardLH.x == approx(0.0f));
        REQUIRE(forwardLH.y == approx(0.0f));
        REQUIRE(forwardLH.z == approx(1.0f));

        Matrix4 viewRH = Matrix4::lookAtRH(eye, target, up);
        Vector3 forwardRH = viewRH.getForward();
        REQUIRE(forwardRH.x == approx(0.0f));
        REQUIRE(forwardRH.y == approx(0.0f));
        REQUIRE(forwardRH.z == approx(-1.0f));
    }

    SECTION("rotation function matches axis extraction")
    {
        using Ark::Math::Matrix4;
        using Ark::Math::Vector3;

        Vector3 const right(1.0f, 0.0f, 0.0f);
        Vector3 const up(0.0f, 1.0f, 0.0f);
        Vector3 const forward(0.0f, 0.0f, 1.0f);

        Matrix4 rotMatrix = Matrix4::rotation(right, up, forward);

        Vector3 extractedRight = rotMatrix.getRight();
        Vector3 extractedUp = rotMatrix.getUp();
        Vector3 extractedForward = rotMatrix.getForward();

        auto approx = [](float v)
        {
            return Catch::Approx(v).margin(1e-5f);
        };

        REQUIRE(extractedRight.x == approx(right.x));
        REQUIRE(extractedRight.y == approx(right.y));
        REQUIRE(extractedRight.z == approx(right.z));

        REQUIRE(extractedUp.x == approx(up.x));
        REQUIRE(extractedUp.y == approx(up.y));
        REQUIRE(extractedUp.z == approx(up.z));

        REQUIRE(extractedForward.x == approx(forward.x));
        REQUIRE(extractedForward.y == approx(forward.y));
        REQUIRE(extractedForward.z == approx(forward.z));
    }

    SECTION("automatic coordinate system dispatch")
    {
        using Ark::float32;
        using Ark::Math::CoordinateSystem;
        using Ark::Math::CurrentCoordinateSystem;
        using Ark::Math::Matrix4;
        using Ark::Math::PiOver4;

        float32 const fov = PiOver4;
        float32 const aspect = 16.0f / 9.0f;
        float32 const nearPlane = 0.1f;
        float32 const farPlane = 100.0f;

        Matrix4 proj = Matrix4::perspectiveFieldOfView(fov, aspect, nearPlane, farPlane);

        if constexpr (CurrentCoordinateSystem == CoordinateSystem::RightHanded)
        {
            REQUIRE(proj.m34 == Catch::Approx(-1.0f));
        }
        else
        {
            REQUIRE(proj.m34 == Catch::Approx(1.0f));
        }
    }

    SECTION("axis extraction right-left-up-down")
    {
        using Ark::Math::Matrix4;
        using Ark::Math::Vector3;

        auto approx = [](float v)
        {
            return Catch::Approx(v).margin(1e-5f);
        };

        Matrix4 identity = Matrix4::Identity;

        Vector3 right = identity.getRight();
        REQUIRE(right.x == approx(1.0f));
        REQUIRE(right.y == approx(0.0f));
        REQUIRE(right.z == approx(0.0f));

        Vector3 left = identity.getLeft();
        REQUIRE(left.x == approx(-1.0f));
        REQUIRE(left.y == approx(0.0f));
        REQUIRE(left.z == approx(0.0f));

        Vector3 up = identity.getUp();
        REQUIRE(up.x == approx(0.0f));
        REQUIRE(up.y == approx(1.0f));
        REQUIRE(up.z == approx(0.0f));

        Vector3 down = identity.getDown();
        REQUIRE(down.x == approx(0.0f));
        REQUIRE(down.y == approx(-1.0f));
        REQUIRE(down.z == approx(0.0f));
    }

    SECTION("axis extraction forward-backward")
    {
        using Ark::Math::Matrix4;
        using Ark::Math::Vector3;

        auto approx = [](float v)
        {
            return Catch::Approx(v).margin(1e-5f);
        };

        Matrix4 identity = Matrix4::Identity;

        Vector3 fw = identity.getForward();
        REQUIRE(fw.x == approx(0.0f));
        REQUIRE(fw.y == approx(0.0f));
        REQUIRE(fw.z == approx(1.0f));

        Vector3 backward = identity.getBackward();
        REQUIRE(backward.x == approx(0.0f));
        REQUIRE(backward.y == approx(0.0f));
        REQUIRE(backward.z == approx(-1.0f));
    }

    SECTION("column constructor preserves w components")
    {
        using Ark::Math::Matrix4;
        using Ark::Math::Vector4;

        Matrix4 const m{
            Vector4{1.0f, 2.0f, 3.0f, 4.0f},
            Vector4{5.0f, 6.0f, 7.0f, 8.0f},
            Vector4{9.0f, 10.0f, 11.0f, 12.0f},
            Vector4{13.0f, 14.0f, 15.0f, 16.0f}};

        REQUIRE(m.m14 == Catch::Approx(4.0f));
        REQUIRE(m.m24 == Catch::Approx(8.0f));
        REQUIRE(m.m34 == Catch::Approx(12.0f));
        REQUIRE(m.m44 == Catch::Approx(16.0f));
    }

    SECTION("frustum keeps m42 at zero")
    {
        using Ark::Math::Matrix4;

        Matrix4 const m = Matrix4::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        REQUIRE(m.m42 == Catch::Approx(0.0f));
    }

    SECTION("transformVector ignores translation")
    {
        using Ark::Math::Matrix4;
        using Ark::Math::Vector3;

        Matrix4 const t = Matrix4::translation(Vector3{100.0f, -50.0f, 25.0f});
        Vector3 const v{1.0f, 2.0f, 3.0f};
        Vector3 const transformed = Matrix4::transformVector(v, t);

        REQUIRE(transformed.x == Catch::Approx(v.x));
        REQUIRE(transformed.y == Catch::Approx(v.y));
        REQUIRE(transformed.z == Catch::Approx(v.z));
    }
}
