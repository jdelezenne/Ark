#pragma once

#include "Ark/Collections/InlineArray.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/Internal/Format.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

// On macOS, MacTypes.h defines nil as a macro which conflicts with our static member
#ifdef nil
#undef nil
#endif

namespace Ark
{
    /// 128-bit universally unique identifier.
    struct UUID final
    {
    public:
        using Bytes = Collections::InlineArray<uint8, 16>;

        /// The nil UUID (all zeros).
        static const UUID nil;

    private:
        Bytes data{};

    public:
        /// Create a nil UUID (all zeros).
        constexpr UUID() = default;

        /// Create from raw bytes.
        explicit constexpr UUID(Bytes const& bytes)
            : data(bytes)
        {
        }

        /// Parse from string input (accepts forms with or without dashes and braces).
        static UUID fromString(StringSlice text);

        /// Convert to canonical string form (8-4-4-4-12 lowercase hex).
        String toString() const;

        /// Access raw bytes.
        constexpr Bytes const& bytes() const
        {
            return data;
        }

        /// True if all bytes are zero.
        constexpr bool isNil() const
        {
            for (usize i = 0; i < 16; ++i)
            {
                if (data[i] != 0)
                {
                    return false;
                }
            }
            return true;
        }

        /// Construct a name-based UUID v5 from a 20-byte SHA-1-like input (first 16 bytes used).
        /// @details This does not compute SHA-1; callers must provide a 20-byte digest.
        static UUID fromHash160(Collections::InlineArray<uint8, 20> const& digest);

        constexpr bool operator==(UUID const& other) const
        {
            for (usize i = 0; i < 16; ++i)
            {
                if (data[i] != other.data[i])
                {
                    return false;
                }
            }
            return true;
        }

        constexpr bool operator!=(UUID const& other) const
        {
            return !(*this == other);
        }
    };

    inline const UUID UUID::nil{};
}

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::UUID, char>
{
    template <typename FormatContext>
    auto format(Ark::UUID const& v, FormatContext& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::format_to(ctx.out(), "{}", v.toString().asPointer());
    }

    constexpr auto parse(ARK_FORMAT_NAMESPACE::format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }
};

namespace std
{
    template <>
    struct hash<Ark::UUID>
    {
        size_t operator()(Ark::UUID const& v) const
        {
            return static_cast<size_t>(Ark::computeHashArray(v.bytes().asPointer(), 16));
        }
    };
}
