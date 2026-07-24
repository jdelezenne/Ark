#pragma once

#include "Ark/Core/Configuration.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#include "Ark/Math/Platform/CppStd/Intrinsics.hpp"
#elif (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#include "Ark/Math/Platform/CStd/Intrinsics.hpp"
#else
#include "Ark/Math/Platform/Generic/Intrinsics.hpp"
#endif
