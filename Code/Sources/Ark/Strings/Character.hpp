#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Strings/AnsiCharacters.hpp"
#include "Ark/Strings/Internal/Format.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#define ARK_CONSTEXPR
#else
#define ARK_CONSTEXPR constexpr
#endif

namespace Ark
{
    /// A single ANSI character.
    struct Character final
    {
    public:
        static constexpr AnsiChar NullCharacter = '\0';

        /// The underlying character type.
        using CharType = uint8;

    private:
        CharType value{};

    public:
#pragma region Constructors and Assignments

        /// Default constructor.
        ARK_CONSTEXPR Character() = default;

        /// Construct a Character from `CharType`.
        /// @param ch The character value.
        ARK_CONSTEXPR explicit Character(CharType ch)
            : value{ch}
        {
        }

        /// Construct a Character from `CharType`.
        /// @param ch The character value.
        ARK_CONSTEXPR Character(char ch)
            : value{static_cast<CharType>(ch)}
        {
        }

        /// Construct a Character from an integer.
        /// @param value The integer value.
        ARK_CONSTEXPR explicit Character(int value)
            : value{static_cast<CharType>(value)}
        {
        }

        ARK_CONSTEXPR Character& operator=(CharType ch)
        {
            value = ch;
            return *this;
        }

        ARK_CONSTEXPR Character& operator=(char ch)
        {
            value = ch;
            return *this;
        }

#pragma endregion

        ARK_CONSTEXPR explicit operator CharType() const
        {
            return value;
        }

        ARK_CONSTEXPR explicit operator char() const
        {
            return value;
        }

        /// Get the underlying char value
        /// @return The char value
        ARK_CONSTEXPR char getValue() const
        {
            return value;
        }

        /// Set the underlying char value
        /// @param c The new char value
        ARK_CONSTEXPR void setValue(CharType ch)
        {
            value = ch;
        }

        /// Set the underlying char value
        /// @param c The new char value
        ARK_CONSTEXPR void setValue(char ch)
        {
            value = static_cast<CharType>(static_cast<AnsiChar>(ch));
        }

#pragma region Static Functions

        /// Check if the character is the null terminator.
        static ARK_CONSTEXPR bool isNull(Character ch)
        {
            return ch == NullCharacter;
        }

        /// Check if the character is alphabetic.
        static ARK_CONSTEXPR bool isAlphabetic(Character ch)
        {
            return Characters::isAlphabetic(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is numeric.
        static ARK_CONSTEXPR bool isNumeric(Character ch)
        {
            return Characters::isNumeric(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is alphanumeric.
        static ARK_CONSTEXPR bool isAlphanumeric(Character ch)
        {
            return Characters::isAlphanumeric(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is ASCII.
        static ARK_CONSTEXPR bool isAscii(Character ch)
        {
            return Characters::isAscii(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is a control character.
        static ARK_CONSTEXPR bool isControl(Character ch)
        {
            return Characters::isControl(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is a digit.
        static ARK_CONSTEXPR bool isDigit(Character ch)
        {
            return Characters::isDigit(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is a hexadecimal digit.
        static ARK_CONSTEXPR bool isHexDigit(Character ch)
        {
            return Characters::isHexDigit(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is a punctuation character.
        static ARK_CONSTEXPR bool isPunctuation(Character ch)
        {
            return Characters::isPunctuation(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is a graphical character.
        static ARK_CONSTEXPR bool isGraphic(Character ch)
        {
            return Characters::isGraphic(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is a whitespace character.
        static ARK_CONSTEXPR bool isWhitespace(Character ch)
        {
            return Characters::isWhitespace(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is uppercase.
        static ARK_CONSTEXPR bool isUppercase(Character ch)
        {
            return Characters::isUppercase(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is lowercase.
        static ARK_CONSTEXPR bool isLowercase(Character ch)
        {
            return Characters::isLowercase(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is an uppercase hexadecimal digit.
        static ARK_CONSTEXPR bool isHexUppercase(Character ch)
        {
            return Characters::isHexUppercase(static_cast<AnsiChar>(ch));
        }

        /// Check if the character is a lowercase hexadecimal digit.
        static ARK_CONSTEXPR bool isHexLowercase(Character ch)
        {
            return Characters::isHexLowercase(static_cast<AnsiChar>(ch));
        }

        /// Convert the character to a digit.
        /// @param ch The character to convert.
        /// @param radix The radix (base) for the conversion (default is 10).
        /// @return The digit value, or -1 if conversion is not possible.
        static ARK_CONSTEXPR int toDigit(Character ch, int radix = 10)
        {
            return Characters::toDigit(static_cast<AnsiChar>(ch), radix);
        }

        /// Convert the character to uppercase.
        static ARK_CONSTEXPR Character toUppercase(Character ch)
        {
            return Characters::toUppercase(static_cast<AnsiChar>(ch));
        }

        /// Convert the character to lowercase.
        static ARK_CONSTEXPR Character toLowercase(Character ch)
        {
            return Characters::toLowercase(static_cast<AnsiChar>(ch));
        }

#pragma endregion

#pragma region Functions
        // clang-format off
        ARK_CONSTEXPR bool isNull() const { return isNull(*this); }
        ARK_CONSTEXPR bool isAlphabetic() const { return isAlphabetic(*this); }
        ARK_CONSTEXPR bool isNumeric() const { return isNumeric(*this); }
        ARK_CONSTEXPR bool isAlphanumeric() const { return isAlphanumeric(*this); }
        ARK_CONSTEXPR bool isAscii() const { return isAscii(*this); }
        ARK_CONSTEXPR bool isControl() const { return isControl(*this); }
        ARK_CONSTEXPR bool isDigit() const { return isDigit(*this); }
        ARK_CONSTEXPR bool isHexDigit() const { return isHexDigit(*this); }
        ARK_CONSTEXPR bool isPunctuation() const { return isPunctuation(*this); }
        ARK_CONSTEXPR bool isGraphic() const { return isGraphic(*this); }
        ARK_CONSTEXPR bool isWhitespace() const { return isWhitespace(*this); }
        ARK_CONSTEXPR bool isUppercase() const { return isUppercase(*this); }
        ARK_CONSTEXPR bool isLowercase() const { return isLowercase(*this); }
        ARK_CONSTEXPR bool isHexUppercase() const { return isHexUppercase(*this); }
        ARK_CONSTEXPR bool isHexLowercase() const { return isHexLowercase(*this); }
        ARK_CONSTEXPR int toDigit(int radix = 10) const { return toDigit(*this, radix); }
        ARK_CONSTEXPR Character toUppercase() const { return toUppercase(*this); }
        ARK_CONSTEXPR Character toLowercase() const { return toLowercase(*this); }
        // clang-format on
#pragma endregion

#pragma region Comparison Operators

        constexpr bool operator==(const Character& other) const
        {
            return value == other.value;
        }

        constexpr bool operator!=(const Character& other) const
        {
            return value != other.value;
        }

        constexpr bool operator<(const Character& other) const
        {
            return value < other.value;
        }

        constexpr bool operator<=(const Character& other) const
        {
            return value <= other.value;
        }

        constexpr bool operator>(const Character& other) const
        {
            return value > other.value;
        }

        constexpr bool operator>=(const Character& other) const
        {
            return value >= other.value;
        }

#pragma endregion
    };
}

#include "Ark/Core/Hasher.hpp"

namespace Ark
{
    template <>
    struct Hasher<Character>
    {
        usize operator()(const Character& value) const
        {
            return computeHashValue(value.getValue());
        }
    };
}

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::Character, char>
{
    template <typename FormatContext>
    auto format(const Ark::Character& v, FormatContext& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::format_to(ctx.out(), "{}", v.getValue());
    }

    constexpr auto parse(ARK_FORMAT_NAMESPACE::format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }
};

#undef ARK_CONSTEXPR
