#pragma once

#include "Ark/Core/Configuration.hpp"
#include "Ark/Core/Platform.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Functions.hpp"

#if defined(ARK_COMPILER_MSVC)
#include <intrin.h>
#endif

namespace Ark
{
#if !defined(ARK_LITTLE_ENDIAN) && !defined(ARK_BIG_ENDIAN)

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define ARK_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define ARK_BIG_ENDIAN
#elif defined(_WIN32) || defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) ||       \
    defined(__x86_64__) || defined(__i386__) || defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM64)
#define ARK_LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
    defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#define ARK_BIG_ENDIAN
#else
#error "Unable to detect platform endianness"
#endif

#endif

    /// Byte order used to represent multi-byte values.
    enum class ByteOrder
    {
        LittleEndian,
        BigEndian,
    };

    /// Returns the native byte order of the current platform.
    /// @return The platform byte order.
    constexpr ByteOrder getNativeByteOrder()
    {
#if defined(ARK_LITTLE_ENDIAN)
        return ByteOrder::LittleEndian;
#elif defined(ARK_BIG_ENDIAN)
        return ByteOrder::BigEndian;
#endif
    }
}

namespace Ark::Internal
{
#if defined(ARK_COMPILER_CLANG) || defined(ARK_COMPILER_GCC)

    inline uint16 swapUInt16(uint16 x)
    {
        return __builtin_bswap16(x);
    }

    inline uint32 swapUInt32(uint32 x)
    {
        return __builtin_bswap32(x);
    }

    inline uint64 swapUInt64(uint64 x)
    {
        return __builtin_bswap64(x);
    }

#elif defined(ARK_COMPILER_MSVC)

    inline uint16 swapUInt16(uint16 x)
    {
        return _byteswap_ushort(x);
    }

    inline uint32 swapUInt32(uint32 x)
    {
        return _byteswap_ulong(x);
    }

    inline uint64 swapUInt64(uint64 x)
    {
        return _byteswap_uint64(x);
    }

#else

    inline uint16 swapUInt16(uint16 x)
    {
        return static_cast<uint16>((x >> 8) | (x << 8));
    }

    inline uint32 swapUInt32(uint32 x)
    {
        uint16 const low = static_cast<uint16>(x);
        uint16 const high = static_cast<uint16>(x >> 16);

        return (static_cast<uint32>(static_cast<uint16>((low >> 8) | (low << 8))) << 16) |
               static_cast<uint16>((high >> 8) | (high << 8));
    }

    inline uint64 swapUInt64(uint64 x)
    {
        uint32 const low = static_cast<uint32>(x);
        uint32 const high = static_cast<uint32>(x >> 32);
        return (static_cast<uint64>(swapUInt32(low)) << 32) | swapUInt32(high);
    }

#endif

#if defined(ARK_LITTLE_ENDIAN)

#define ARK_ToLittleEndian16(x) ((uint16)(x))
#define ARK_ToLittleEndian32(x) ((uint32)(x))
#define ARK_ToLittleEndian64(x) ((uint64)(x))

#define ARK_ToBigEndian16(x) swapUInt16(x)
#define ARK_ToBigEndian32(x) swapUInt32(x)
#define ARK_ToBigEndian64(x) swapUInt64(x)

#elif defined(ARK_BIG_ENDIAN)

#define ARK_ToLittleEndian16(x) swapUInt16(x)
#define ARK_ToLittleEndian32(x) swapUInt32(x)
#define ARK_ToLittleEndian64(x) swapUInt64(x)

#define ARK_ToBigEndian16(x) ((uint16)(x))
#define ARK_ToBigEndian32(x) ((uint32)(x))
#define ARK_ToBigEndian64(x) ((uint64)(x))

#endif

    constexpr uint16 swap(uint16 value)
    {
        return static_cast<uint16>(((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8));
    }

    constexpr uint32 swap(uint32 value)
    {
        return ((value & 0xFF000000u) >> 24) |
               ((value & 0x00FF0000u) >> 8) |
               ((value & 0x0000FF00u) << 8) |
               ((value & 0x000000FFu) << 24);
    }

    constexpr uint64 swap(uint64 value)
    {
        return ((value >> 56) & 0x00000000000000FFull) |
               ((value >> 40) & 0x000000000000FF00ull) |
               ((value >> 24) & 0x0000000000FF0000ull) |
               ((value >> 8) & 0x00000000FF000000ull) |
               ((value << 8) & 0x000000FF00000000ull) |
               ((value << 24) & 0x0000FF0000000000ull) |
               ((value << 40) & 0x00FF000000000000ull) |
               ((value << 56) & 0xFF00000000000000ull);
    }
}

namespace Ark::ByteSwap
{
    /// Swaps the byte order of a 16-bit unsigned integer.
    /// @param value The value to swap.
    /// @return The byte-swapped value.
    inline uint16 swapUInt16(uint16 value)
    {
        return Internal::swapUInt16(value);
    }

    /// Swaps the byte order of a 32-bit unsigned integer.
    /// @param value The value to swap.
    /// @return The byte-swapped value.
    inline uint32 swapUInt32(uint32 value)
    {
        return Internal::swapUInt32(value);
    }

    /// Swaps the byte order of a 64-bit unsigned integer.
    /// @param value The value to swap.
    /// @return The byte-swapped value.
    inline uint64 swapUInt64(uint64 value)
    {
        return Internal::swapUInt64(value);
    }

    /// Swaps the byte order of a 16-bit signed integer.
    /// @param value The value to swap.
    /// @return The byte-swapped value.
    inline int16 swapInt16(int16 value)
    {
        return static_cast<int16>(Internal::swapUInt16(static_cast<uint16>(value)));
    }

    /// Swaps the byte order of a 32-bit signed integer.
    /// @param value The value to swap.
    /// @return The byte-swapped value.
    inline int32 swapInt32(int32 value)
    {
        return static_cast<int32>(Internal::swapUInt32(static_cast<uint32>(value)));
    }

    /// Swaps the byte order of a 64-bit signed integer.
    /// @param value The value to swap.
    /// @return The byte-swapped value.
    inline int64 swapInt64(int64 value)
    {
        return static_cast<int64>(Internal::swapUInt64(static_cast<uint64>(value)));
    }

    /// Swaps the byte order of a single-precision float.
    /// @param value The value to swap.
    /// @return The byte-swapped value.
    inline float swapFloat(float value)
    {
        uint32 bits;
        Memory::copy(&value, &bits, sizeof(bits));
        bits = Internal::swapUInt32(bits);

        float result;
        Memory::copy(&bits, &result, sizeof(result));
        return result;
    }

    /// Swaps the byte order of a double-precision float.
    /// @param value The value to swap.
    /// @return The byte-swapped value.
    inline float64 swapDouble(float64 value)
    {
        uint64 bits;
        Memory::copy(&value, &bits, sizeof(bits));
        bits = Internal::swapUInt64(bits);

        float64 result;
        Memory::copy(&bits, &result, sizeof(result));
        return result;
    }

#if defined(ARK_LITTLE_ENDIAN)
    /// Converts a 16-bit unsigned integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint16 toLittleEndianUInt16(uint16 value)
    {
        return value;
    }

    /// Converts a 32-bit unsigned integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint32 toLittleEndianUInt32(uint32 value)
    {
        return value;
    }

    /// Converts a 64-bit unsigned integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint64 toLittleEndianUInt64(uint64 value)
    {
        return value;
    }

    /// Converts a 16-bit signed integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int16 toLittleEndianInt16(int16 value)
    {
        return value;
    }

    /// Converts a 32-bit signed integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int32 toLittleEndianInt32(int32 value)
    {
        return value;
    }

    /// Converts a 64-bit signed integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int64 toLittleEndianInt64(int64 value)
    {
        return value;
    }

    /// Converts a single-precision float to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline float32 toLittleEndianFloat(float32 value)
    {
        return value;
    }

    /// Converts a double-precision float to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline float64 toLittleEndianDouble(float64 value)
    {
        return value;
    }

    /// Converts a 16-bit unsigned integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint16 toBigEndianUInt16(uint16 value)
    {
        return swapUInt16(value);
    }

    /// Converts a 32-bit unsigned integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint32 toBigEndianUInt32(uint32 value)
    {
        return swapUInt32(value);
    }

    /// Converts a 64-bit unsigned integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint64 toBigEndianUInt64(uint64 value)
    {
        return swapUInt64(value);
    }

    /// Converts a 16-bit signed integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int16 toBigEndianInt16(int16 value)
    {
        return swapInt16(value);
    }

    /// Converts a 32-bit signed integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int32 toBigEndianInt32(int32 value)
    {
        return swapInt32(value);
    }

    /// Converts a 64-bit signed integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int64 toBigEndianInt64(int64 value)
    {
        return swapInt64(value);
    }

    /// Converts a single-precision float to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline float32 toBigEndianFloat(float32 value)
    {
        return swapFloat(value);
    }

    /// Converts a double-precision float to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline float64 toBigEndianDouble(float64 value)
    {
        return swapDouble(value);
    }
#else
    /// Converts a 16-bit unsigned integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint16 toLittleEndianUInt16(uint16 value)
    {
        return swapUInt16(value);
    }

    /// Converts a 32-bit unsigned integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint32 toLittleEndianUInt32(uint32 value)
    {
        return swapUInt32(value);
    }

    /// Converts a 64-bit unsigned integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint64 toLittleEndianUInt64(uint64 value)
    {
        return swapUInt64(value);
    }

    /// Converts a 16-bit signed integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int16 toLittleEndianInt16(int16 value)
    {
        return swapInt16(value);
    }

    /// Converts a 32-bit signed integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int32 toLittleEndianInt32(int32 value)
    {
        return swapInt32(value);
    }

    /// Converts a 64-bit signed integer to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int64 toLittleEndianInt64(int64 value)
    {
        return swapInt64(value);
    }

    /// Converts a single-precision float to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline float32 toLittleEndianFloat(float32 value)
    {
        return swapFloat(value);
    }

    /// Converts a double-precision float to little-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline float64 toLittleEndianDouble(float64 value)
    {
        return swapDouble(value);
    }

    /// Converts a 16-bit unsigned integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint16 toBigEndianUInt16(uint16 value)
    {
        return value;
    }

    /// Converts a 32-bit unsigned integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint32 toBigEndianUInt32(uint32 value)
    {
        return value;
    }

    /// Converts a 64-bit unsigned integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline uint64 toBigEndianUInt64(uint64 value)
    {
        return value;
    }

    /// Converts a 16-bit signed integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int16 toBigEndianInt16(int16 value)
    {
        return value;
    }

    /// Converts a 32-bit signed integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int32 toBigEndianInt32(int32 value)
    {
        return value;
    }

    /// Converts a 64-bit signed integer to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline int64 toBigEndianInt64(int64 value)
    {
        return value;
    }

    /// Converts a single-precision float to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline float32 toBigEndianFloat(float32 value)
    {
        return value;
    }

    /// Converts a double-precision float to big-endian.
    /// @param value The value to convert.
    /// @return The converted value.
    inline float64 toBigEndianDouble(float64 value)
    {
        return value;
    }
#endif
}
