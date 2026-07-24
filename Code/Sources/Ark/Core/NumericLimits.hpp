#pragma once

#include "Ark/Core/Configuration.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#include "Ark/Core/Platform/CppStd/NumericLimits.hpp"
#elif (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#include "Ark/Core/Platform/CStd/NumericLimits.hpp"
#else
#include "Ark/Core/Platform/Generic/NumericLimits.hpp"
#endif
