#include "Ark/Math/Matrix4.hpp"
#include "Ark/Math/Quaternion.hpp"
#include "Ark/Math/Vector2.hpp"
#include "Ark/Math/Vector3.hpp"
#include "Ark/Math/Vector4.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"

using namespace Ark;
using namespace Ark::Math;

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    Vector2 v2{3.0f, 4.0f};
    Ark::System::Console::printlnf("Vector2 length: %.3f", static_cast<float64>(v2.getLength()));

    Vector3 a3{1.0f, 0.0f, 0.0f};
    Vector3 b3{0.0f, 1.0f, 0.0f};
    Ark::float32 dot = Vector3::dot(a3, b3);
    Vector3 cross = Vector3::cross(a3, b3);
    Ark::System::Console::printlnf("Vector3 dot: %.3f, cross: (%.3f, %.3f, %.3f)",
                                   static_cast<float64>(dot),
                                   static_cast<float64>(cross.x),
                                   static_cast<float64>(cross.y),
                                   static_cast<float64>(cross.z));

    Matrix4 translation = Matrix4::translation({2.0f, 3.0f, 4.0f});
    Vector4 p = translation.transformPoint({1.0f, 2.0f, 3.0f});
    Ark::System::Console::printlnf("Matrix4 translate point -> (%.3f, %.3f, %.3f, %.3f)",
                                   static_cast<float64>(p.x),
                                   static_cast<float64>(p.y),
                                   static_cast<float64>(p.z),
                                   static_cast<float64>(p.w));

    Quaternion q = Quaternion::fromEulerAngles(0.0f, Ark::Math::degreesToRadians(90.0f), 0.0f);
    Vector3 rotated = q.transformVector({1.0f, 0.0f, 0.0f});
    Ark::System::Console::printlnf("Quaternion rotate X by yaw 90deg -> (%.3f, %.3f, %.3f)",
                                   static_cast<float64>(rotated.x),
                                   static_cast<float64>(rotated.y),
                                   static_cast<float64>(rotated.z));

    return true;
}
