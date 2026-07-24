#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark
{
    template <typename Key>
    struct Hasher;

    namespace Internal
    {
        /// Conditionally enabled hash function.
        /// @tparam Key The key type to hash.
        /// @tparam Enabled Whether the hash function is enabled.
        /// @note This is a helper struct to enable or disable the hash function based on the key type.
        template <typename Key, bool Enabled>
        struct ConditionallyEnabledHash
        {
            usize operator()(const Key& value) const
            {
                return Hasher<Key>::compute(value);
            }
        };

        template <typename Key>
        struct ConditionallyEnabledHash<Key, false>
        {
            ConditionallyEnabledHash() = delete;

            ConditionallyEnabledHash(const ConditionallyEnabledHash&) = delete;
            ConditionallyEnabledHash(ConditionallyEnabledHash&&) = delete;

            ConditionallyEnabledHash& operator=(const ConditionallyEnabledHash&) = delete;
            ConditionallyEnabledHash& operator=(ConditionallyEnabledHash&&) = delete;
        };

#pragma region Fnv1a

#if defined(ARK_ARCHITECTURE_X64) || defined(ARK_ARCHITECTURE_ARM64)
        // 64-bit FNV-1a
        inline constexpr usize fnvOffsetBasis = 14695981039346656037ULL;
        inline constexpr usize fnvPrime = 1099511628211ULL;
#elif defined(ARK_ARCHITECTURE_X86)
        // 32-bit FNV-1a
        inline constexpr usize fnvOffsetBasis = 2166136261U;
        inline constexpr usize fnvPrime = 16777619U;
#else
#error "Unsupported architecture for FNV-1a"
#endif

        /// Computes the FNV-1a hash value for a range of bytes.
        /// @param value The partial hash value.
        /// @param first The first byte in the range.
        /// @param count The number of bytes in the range.
        /// @return The computed FNV-1a hash value.
        /// @note This function is used to accumulate a range of bytes into a partial FNV-1a hash value.
        inline usize computeFnv1aHashBytes(usize value, const uint8* const first, const usize count)
        {
            // Accumulate range [first, first + count) into partial FNV-1a hash value
            for (usize index = 0; index < count; ++index)
            {
                // Bitwise XOR the bottom with the current byte
                value ^= static_cast<usize>(first[index]);
                value *= Internal::fnvPrime;
            }

            return value;
        }

        /// Computes the FNV-1a hash value for a range of elements.
        /// @tparam Type The element type.
        /// @param value The partial hash value.
        /// @param first The first element in the range.
        /// @param last The element past the last element in the range.
        /// @return The computed FNV-1a hash value.
        /// @note This function is used to accumulate a range of elements into a partial FNV-1a hash value.
        template <Concepts::TriviallyCopyable Type>
        usize computeFnv1aHashRange(const usize value, Type const* first, Type const* last)
        {
            // Accumulate range [first, last) into partial FNV-1a hash value
            const auto firstByte = reinterpret_cast<const uint8*>(first);
            const auto lastByte = reinterpret_cast<const uint8*>(last);

            return computeFnv1aHashBytes(value, firstByte, static_cast<usize>(lastByte - firstByte));
        }

        /// Computes the FNV-1a hash value for a key value.
        /// @tparam Key The key type.
        /// @param hashValue The partial hash value.
        /// @param keyValue The key value.
        /// @return The computed FNV-1a hash value.
        /// @note This function is used to accumulate a key value into a partial FNV-1a hash value.
        template <Concepts::TriviallyCopyable Key>
        usize computeFnv1aHashValue(const usize hashValue, const Key& keyValue)
        {
            return computeFnv1aHashBytes(hashValue, &reinterpret_cast<const uint8&>(keyValue), sizeof(Key));
        }
    }

#pragma endregion

    /// Computes the FNV-1a hash value for a key value.
    /// @tparam Key The key type.
    /// @param value The key value.
    /// @return The computed FNV-1a hash value.
    template <Concepts::TriviallyCopyable Key>
    usize computeHashValue(const Key& value)
    {
        return Internal::computeFnv1aHashValue(Internal::fnvOffsetBasis, value);
    }

    template <Concepts::TriviallyCopyable Key>
    usize computeHashArray(const Key* const first, const usize count)
    {
        return Internal::computeFnv1aHashBytes(
            Internal::fnvOffsetBasis, reinterpret_cast<const uint8*>(first), count * sizeof(Key));
    }

    /// Hasher function for integral, pointer, and enum types.
    /// @tparam Key The key type to hash.
    template <typename Key>
    struct Hasher
        : Internal::ConditionallyEnabledHash<
              Key,
              !Traits::isConst<Key> &&
                  !Traits::isVolatile<Key> &&
                  (Traits::isEnum<Key> ||
                   Traits::isIntegral<Key> ||
                   Traits::isPointer<Key>)>
    {
        static usize compute(const Key& value)
        {
            return computeHashValue(value);
        }

        usize operator()(const Key& value) const
        {
            return computeHashValue(value);
        }
    };

    /// Specialized hash function for `float32` values.
    template <>
    struct Hasher<float32>
    {
        usize operator()(const float32 value) const
        {
            return computeHashValue(value == 0.0F ? 0.0F : value); // map -0 to 0
        }
    };

    /// Specialized hash function for `float64` values.
    template <>
    struct Hasher<float64>
    {
        usize operator()(const float64 value) const
        {
            return computeHashValue(value == 0.0 ? 0.0 : value); // map -0 to 0
        }
    };

    /// Specialized hash function for `nullptr` values.
    template <>
    struct Hasher<NullType>
    {
        usize operator()(NullType) const
        {
            void* nullPtr{};
            return computeHashValue(nullPtr);
        }
    };

    /// Computes the FNV-1a hash for a given string.
    /// @param str The input string to hash.
    /// @param value The initial hash value (default is the FNV-1a offset basis).
    /// @return The 64-bit FNV-1a hash of the input string.
    /// @details This function uses the FNV-1a hash algorithm to compute a 64-bit hash value for the input string.
    constexpr uint64 computeFnv1a(char const* str, uint64 value = Internal::fnvOffsetBasis)
    {
        return (str[0] == '\0') ? value : computeFnv1a(&str[1], (value ^ uint64(str[0])) * Internal::fnvPrime);
    }

    /// Computes the FNV-1a hash for a given string.
    /// @param str The input string to hash.
    /// @param  The initial hash value (default is the FNV-1a offset basis).
    /// @return The 64-bit FNV-1a hash of the input string.
    constexpr uint64 operator""_hash(char const* str, usize)
    {
        return computeFnv1a(str);
    }
}
