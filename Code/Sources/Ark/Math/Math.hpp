/// @file
/// Convenience header that includes all Ark math types and utilities.

#pragma once

#include "Ark/Core/Configuration.hpp"
#include "Ark/Math/Angle.hpp"
#include "Ark/Math/Bezier.hpp"
#include "Ark/Math/BoundingBox.hpp"
#include "Ark/Math/BoundingRect.hpp"
#include "Ark/Math/Box.hpp"
#include "Ark/Math/Circle.hpp"
#include "Ark/Math/Constants.hpp"
#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Intrinsics.hpp"
#include "Ark/Math/Line.hpp"
#include "Ark/Math/Matrix3.hpp"
#include "Ark/Math/Matrix4.hpp"
#include "Ark/Math/PcgRandom.hpp"
#include "Ark/Math/Plane.hpp"
#include "Ark/Math/Point2.hpp"
#include "Ark/Math/Polygon.hpp"
#include "Ark/Math/Quaternion.hpp"
#include "Ark/Math/Random.hpp"
#include "Ark/Math/RandomUtilities.hpp"
#include "Ark/Math/Ray.hpp"
#include "Ark/Math/Rect.hpp"
#include "Ark/Math/Size2.hpp"
#include "Ark/Math/Size3.hpp"
#include "Ark/Math/Sphere.hpp"
#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#include "Ark/Math/StdLibRandom.hpp"
#endif
#include "Ark/Math/Triangle.hpp"
#include "Ark/Math/Vector2.hpp"
#include "Ark/Math/Vector3.hpp"
#include "Ark/Math/Vector4.hpp"
