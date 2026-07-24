#pragma once

#include "Ark/Concurrency/Types.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Platform.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"

#if defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Concurrency/Platform/Windows/Atomic.hpp"
#elif defined(ARK_PLATFORM_POSIX)
#include "Ark/Concurrency/Platform/Posix/Atomic.hpp"
#else
#error "Ark::Concurrency::Atomic is not implemented for this platform"
#endif
