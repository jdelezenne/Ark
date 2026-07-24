#pragma once

#include "Ark/Core/Configuration.hpp"
#include "Ark/Core/Platform.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#include <cstddef>
#include <cstdint>
#elif (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#include <stddef.h>
#include <stdint.h>
#endif

namespace Ark
{
#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)

    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    using byte = std::byte;

    using uint = std::size_t;
    using intptr = std::intptr_t;
    using uintptr = std::uintptr_t;
    using isize = std::ptrdiff_t;
    using usize = std::size_t;

#elif (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)

    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;

    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;

    using byte = uint8_t;

    using uint = size_t;
    using intptr = intptr_t;
    using uintptr = uintptr_t;
    using isize = ptrdiff_t;
    using usize = size_t;

#else

    using int8 = signed char;
    using int16 = short;
    using int32 = int;
    using int64 = long long;

    using uint8 = unsigned char;
    using uint16 = unsigned short;
    using uint32 = unsigned int;
    using uint64 = unsigned long long;

    using byte = unsigned char;

#if defined(ARK_COMPILER_MSVC)

#if defined(ARK_ARCHITECTURE_X64)
    using uint = unsigned __int64;
    using intptr = __int64;
    using uintptr = unsigned __int64;
    using isize = __int64;
    using usize = unsigned __int64;
#elif defined(ARK_ARCHITECTURE_X86)
    using uint = unsigned int;
    using intptr = int;
    using uintptr = unsigned int;
    using isize = int;
    using usize = unsigned int;
#else
#error "Unsupported architecture"
#endif

#else

#if defined(ARK_PLATFORM_WINDOWS)
// Windows LLP64 model
#if defined(ARK_ARCHITECTURE_X64) || defined(ARK_ARCHITECTURE_ARM64)
    using uint = unsigned long long;
    using intptr = long long;
    using uintptr = unsigned long long;
    using isize = long long;
    using usize = unsigned long long;
#elif defined(ARK_ARCHITECTURE_X86)
    using uint = unsigned int;
    using intptr = int;
    using uintptr = unsigned int;
    using isize = int;
    using usize = unsigned int;
#else
#error "Unsupported architecture"
#endif
#elif defined(ARK_PLATFORM_WEBASSEMBLY)
// Emscripten: size_t/long are unsigned long / long even for wasm32.
    using uint = unsigned int;
    using intptr = long;
    using uintptr = unsigned long;
    using isize = long;
    using usize = unsigned long;
#else
// POSIX LP64 model (macOS/Linux)
#if defined(ARK_ARCHITECTURE_X64) || defined(ARK_ARCHITECTURE_ARM64)
    using uint = unsigned long;
    using intptr = long;
    using uintptr = unsigned long;
    using isize = long;
    using usize = unsigned long;
#elif defined(ARK_ARCHITECTURE_X86)
    using uint = unsigned int;
    using intptr = int;
    using uintptr = unsigned int;
    using isize = int;
    using usize = unsigned int;
#else
#error "Unsupported architecture"
#endif
#endif

#endif

#endif

    using float32 = float;
    using float64 = double;
    using float128 = long double;

#pragma region Aliases

    /// Signed integer types
    using Int8 = int8;   ///< 8-bit signed integer
    using Int16 = int16; ///< 16-bit signed integer
    using Int32 = int32; ///< 32-bit signed integer
    using Int64 = int64; ///< 64-bit signed integer

    /// Unsigned integer types
    using UInt8 = uint8;   ///< 8-bit unsigned integer
    using UInt16 = uint16; ///< 16-bit unsigned integer
    using UInt32 = uint32; ///< 32-bit unsigned integer
    using UInt64 = uint64; ///< 64-bit unsigned integer

    /// Address and size types
    using Int = int;         ///< Signed integer (usuall 32-bit)
    using UInt = uint;       ///< Unsigned integer (usually 32-bit)
    using IntPtr = intptr;   ///< Signed integer for pointers (32-bit or 64-bit)
    using UIntPtr = uintptr; ///< Unsigned integer for pointers (32-bit or 64-bit)

    ///  Floating-point types
    using Float32 = float32; ///< Single-precision floating-point number
    using Float64 = float64; ///< Double-precision floating-point number

    ///  Character types
    using Char = char;            ///< Character type
    using AnsiChar = char;        ///< ANSI character type
    using WideChar = wchar_t;     ///< Wide character type
    using UnicodeChar = char32_t; ///< Unicode character type

    /// Special types
    using Bool = bool;                  ///< Boolean type
    using NullType = decltype(nullptr); ///< Null pointer type

    /// Void type
    struct Void final
    {
    };

#pragma endregion

#pragma region Short Aliases

    using i8 = int8;
    using i16 = int16;
    using i32 = int32;
    using i64 = int64;

    using u8 = uint8;
    using u16 = uint16;
    using u32 = uint32;
    using u64 = uint64;

    using f32 = float32;
    using f64 = float64;

#pragma endregion

    struct InPlace final
    {
        explicit InPlace() = default;
    };

    inline constexpr InPlace inPlace{};
}
