#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark
{
    template <Concepts::Arithmetic T>
    struct Point2;

    using Point2Int = Point2<int32>;
    using Point2UInt = Point2<uint32>;
    using Point2Float = Point2<float32>;

    template <Concepts::Arithmetic T>
    struct Size2;

    template <Concepts::Arithmetic T>
    struct Rect;
}

namespace Ark::Math
{
    /// Specifies the handedness of a coordinate system.
    enum class CoordinateSystem
    {
        LeftHanded,  ///< Left-handed coordinate system (e.g. Direct3D).
        RightHanded, ///< Right-handed coordinate system (e.g. OpenGL).
    };

    /// The coordinate system convention used by this library.
    static constexpr CoordinateSystem CurrentCoordinateSystem = CoordinateSystem::RightHanded;

    struct Angle;

    struct BoundingBox;
    struct BoundingRect;

    struct Box;
    struct Circle;
    struct Plane;
    struct Sphere;

    template <typename T>
    struct Line;

    struct Matrix3;
    struct Matrix4;

    struct Quaternion;

    struct Vector2;
    struct Vector3;
    struct Vector4;
}
