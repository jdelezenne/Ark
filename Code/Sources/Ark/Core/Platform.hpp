#pragma once

// Build configuration
#if defined(DEBUG) || defined(_DEBUG)
#define ARK_BUILD_DEBUG
#else
#define ARK_BUILD_RELEASE
#endif

// Platform detection
#if !defined(ARK_PLATFORM_WINDOWS) && \
    !defined(ARK_PLATFORM_APPLE) &&   \
    !defined(ARK_PLATFORM_MACOS) &&   \
    !defined(ARK_PLATFORM_LINUX) &&   \
    !defined(ARK_PLATFORM_POSIX) &&   \
    !defined(ARK_PLATFORM_WEBASSEMBLY)

#if defined(_WIN32)
#define ARK_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#define ARK_PLATFORM_APPLE
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define ARK_PLATFORM_IOS
#elif defined(TARGET_OS_MAC) && TARGET_OS_MAC
#define ARK_PLATFORM_MACOS
#else
#error "Unsupported Apple platform"
#endif
#elif defined(__EMSCRIPTEN__)
#define ARK_PLATFORM_LINUX
#define ARK_PLATFORM_WEBASSEMBLY
#elif defined(__linux__)
#define ARK_PLATFORM_LINUX
#else
#error "Unsupported platform"
#endif

#endif

// Normalize derived platform macros when the build system predefines a primary platform.
#if (defined(ARK_PLATFORM_MACOS) || defined(ARK_PLATFORM_IOS)) && !defined(ARK_PLATFORM_APPLE)
#define ARK_PLATFORM_APPLE
#endif

#if (defined(ARK_PLATFORM_MACOS) || defined(ARK_PLATFORM_IOS) || defined(ARK_PLATFORM_LINUX)) && \
    !defined(ARK_PLATFORM_POSIX)
#define ARK_PLATFORM_POSIX
#endif

#if defined(__EMSCRIPTEN__) && !defined(ARK_PLATFORM_WEBASSEMBLY)
#define ARK_PLATFORM_WEBASSEMBLY
#endif

// Architecture detection
#if !defined(ARK_ARCHITECTURE_X86) && \
    !defined(ARK_ARCHITECTURE_X64) && \
    !defined(ARK_ARCHITECTURE_ARM64)

#if defined(__wasm64__)
#define ARK_ARCHITECTURE_X64
#elif defined(__wasm32__) || defined(__wasm__)
#define ARK_ARCHITECTURE_X86
#elif defined(__x86_64__) || defined(_M_X64)
#define ARK_ARCHITECTURE_X64
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define ARK_ARCHITECTURE_X86
#elif defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
#define ARK_ARCHITECTURE_ARM64
#else
#error "Unsupported architecture"
#endif

#endif

// Compiler detection
#if !defined(ARK_COMPILER_MSVC) &&  \
    !defined(ARK_COMPILER_CLANG) && \
    !defined(ARK_COMPILER_GCC)

#if defined(_MSC_VER)
#define ARK_COMPILER_MSVC
#elif defined(__clang__)
#define ARK_COMPILER_CLANG
#elif defined(__GNUC__)
#define ARK_COMPILER_GCC
#else
#error "Unsupported compiler"
#endif

#endif

#if defined(ARK_COMPILER_MSVC)

#if __has_cpp_attribute(msvc::intrinsic)
#define ARK_INTRINSIC [[msvc::intrinsic]]
#endif

#endif

#if defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Core/Platform/Windows/Platform.hpp"
#endif