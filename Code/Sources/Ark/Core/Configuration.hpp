#pragma once

/// Uses Ark's generic utility implementation.
#define ARK_STANDARD_LIBRARY_GENERIC 0
/// Uses C++ standard library backed implementation.
#define ARK_STANDARD_LIBRARY_CPP 1
/// Uses C standard library backed implementation.
#define ARK_STANDARD_LIBRARY_C 2

/// Selects which standard library backend Ark should use.
#ifndef ARK_USE_STANDARD_LIBRARY
#define ARK_USE_STANDARD_LIBRARY ARK_STANDARD_LIBRARY_GENERIC
#endif
/// Enables standard library function forwarding utilities.
#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#define ARK_FUNCTION_USE_STANDARD_LIBRARY
#endif

/// Default allocator type name used by core containers.
#define ARK_DEFAULT_ALLOCATOR StandardAllocator
/// Controls usage of hardware memory barriers on Windows.
#define ARK_WINDOWS_USE_HARDWARE_BARRIERS 0
