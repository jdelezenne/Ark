#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Strings/AnsiCharacters.hpp"
#include "Ark/Strings/AnsiStrings.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Strings
{
    namespace Internal
    {
        constexpr int absInt(int value)
        {
            return value < 0 ? -value : value;
        }

        /// Freestanding base-10 power (avoids Math/Intrinsics → <cmath>).
        inline float64 pow10(int exp)
        {
            float64 result = 1.0;
            float64 base = 10.0;

            bool const isNegative = exp < 0;
            exp = isNegative ? -exp : exp;

            while (exp != 0)
            {
                if ((exp & 1) != 0)
                {
                    result *= base;
                }

                base *= base;
                exp >>= 1;
            }

            return isNegative ? 1.0 / result : result;
        }

        template <Concepts::Integral IntType>
        Option<IntType> toInt(StringSlice const& str, bool ignoreTrailing, usize& index)
        {
            if (str.isEmpty())
            {
                return none;
            }

            const usize length = str.getLength();

            usize i = 0;

            // Skip leading whitespaces
            while (i < length && Characters::isWhitespace(str[i]))
            {
                i++;
            }

            bool isNegative = false;

            // Handle sign for signed types
            if constexpr (Traits::isSigned<IntType>)
            {
                if (str[i] == '-')
                {
                    isNegative = true;
                    i++;
                }
                else if (str[i] == '+')
                {
                    i++;
                }
            }

            // Check if there are any digits after the sign
            if (i == length)
            {
                return none;
            }

            constexpr IntType minValue = NumericLimits<IntType>::min();
            constexpr IntType maxValue = NumericLimits<IntType>::max();

            IntType result = 0;

            for (; i < length; i++)
            {
                if (!Characters::isDigit(str[i]))
                {
                    if (ignoreTrailing)
                    {
                        index = i;
                        return result;
                    }

                    // Skip trailing whitespaces
                    while (i < length && Characters::isWhitespace(str[i]))
                    {
                        i++;
                    }

                    if (i < length)
                    {
                        return none;
                    }

                    index = i;
                    return result;
                }

                IntType digit = static_cast<IntType>(str[i] - '0');

                // Check for overflow
                if (result > (maxValue - digit) / 10)
                {
                    // Special case for the last digit of the minimum value
                    if constexpr (Traits::isSigned<IntType>)
                    {
                        if (isNegative && result == (-(minValue / 10)) &&
                            digit == (-(minValue % 10)) && i == length - 1)
                        {
                            result = minValue;
                            break;
                        }
                    }

                    return none;
                }

                result = result * 10 + digit;
            }

            if constexpr (Traits::isSigned<IntType>)
            {
                if (isNegative)
                {
                    if (result == minValue)
                    {
                        index = i;
                        return minValue;
                    }

                    result = -result;
                }
            }

            index = i;
            return result;
        }

        template <Concepts::Integral IntType>
        Option<IntType> toInt(const StringSlice& str, bool ignoreTrailing)
        {
            usize index;
            return Internal::toInt<IntType>(str, ignoreTrailing, index);
        }
    }

    template <Concepts::Integral IntType>
    Option<IntType> toInt(const StringSlice& str, int radix = 10)
    {
        if (str.isEmpty())
        {
            return none;
        }

        if (radix < 2 || radix > 36)
        {
            return none;
        }

        usize const length = str.getLength();

        IntType result = 0;
        bool isNegative = false;
        isize i = 0;

        // Skip leading whitespaces
        while (i < length && Characters::isWhitespace(str[i]))
        {
            i++;
        }

        if (i >= length)
        {
            return none;
        }

        // Handle sign for signed types
        if constexpr (Traits::isSigned<IntType>)
        {
            if (str[i] == '-')
            {
                isNegative = true;
                i++;
            }
            else if (str[i] == '+')
            {
                i++;
            }
        }

        // Handle radix prefix (0x for hex, 0b for binary)
        if (length > i + 1 && str[i] == '0')
        {
            if ((str[i + 1] == 'x' || str[i + 1] == 'X') && radix == 16)
            {
                i += 2;
            }
            else if ((str[i + 1] == 'b' || str[i + 1] == 'B') && radix == 2)
            {
                i += 2;
            }
        }

        // Check if there are any digits after the sign/prefix
        if (i == length)
        {
            return none;
        }

        constexpr IntType minValue = NumericLimits<IntType>::min();
        constexpr IntType maxValue = NumericLimits<IntType>::max();

        for (; i < length; i++)
        {
            if (Characters::isWhitespace(str[i]))
            {
                // Skip trailing whitespaces
                while (i < length && Characters::isWhitespace(str[i]))
                {
                    i++;
                }

                return i < length ? none : Option<IntType>(result);
            }

            int digit = Characters::toDigit(str[i], radix);

            if (digit == -1 || digit >= radix)
            {
                return none;
            }

            // Check for overflow
            if (result > (maxValue - digit) / radix)
            {
                // Special case for the last digit of the minimum value
                if constexpr (Traits::isSigned<IntType>)
                {
                    if (isNegative && result == (-(minValue / radix)) &&
                        digit == (-(minValue % radix)) && i == str.getLength() - 1)
                    {
                        result = minValue;
                        break;
                    }
                }
                return none;
            }

            result = result * radix + digit;
        }

        if constexpr (Traits::isSigned<IntType>)
        {
            if (isNegative)
            {
                if (result == minValue)
                {
                    return result;
                }

                result = -result;
            }
        }

        return result;
    }

    template <Concepts::Integral IntType>
    Option<String> fromInt(IntType value)
    {
        if (value == 0)
        {
            return String("0");
        }

        String result;
        bool isNegative = false;

        if constexpr (Traits::isSigned<IntType>)
        {
            if (value < 0)
            {
                // Handle the edge case of minimum value
                if (value == NumericLimits<IntType>::min())
                {
                    using UnsignedType = Traits::MakeUnsignedType<IntType>;
                    UnsignedType const magnitude = static_cast<UnsignedType>(value);
                    result = fromInt(magnitude).getValueOr("");
                    return "-" + result;
                }

                isNegative = true;
                value = -value;
            }
        }

        while (value > 0)
        {
            result = static_cast<char>('0' + (value % 10)) + result;
            value /= 10;
        }

        if (isNegative)
        {
            result = "-" + result;
        }

        return result;
    }

    template <Concepts::Integral IntType>
    Option<String> fromInt(IntType value, int radix, Casing casing = Casing::Lowercase)
    {
        if (radix < 2 || radix > 36)
        {
            return none;
        }

        if (value == 0)
        {
            return String("0");
        }

        String result;
        bool isNegative = false;

        if constexpr (Traits::isSigned<IntType>)
        {
            if (value < 0)
            {
                // Handle the edge case of minimum value for signed types
                if (value == NumericLimits<IntType>::min())
                {
                    using UnsignedType = Traits::MakeUnsignedType<IntType>;
                    UnsignedType const magnitude = static_cast<UnsignedType>(value);
                    result = fromInt(magnitude, radix, casing).getValueOr("");
                    return "-" + result;
                }

                isNegative = true;
                value = -value;
            }
        }

        while (value > 0)
        {
            int digit = value % radix;
            result = Characters::digitToChar(digit, radix, casing) + result;
            value /= radix;
        }

        if (isNegative)
        {
            result = "-" + result;
        }

        return result;
    }

    template <Concepts::FloatingPoint FloatType>
    Option<FloatType> toFloat(const StringSlice& str)
    {
        if (str.isEmpty())
        {
            return none;
        }

        const usize length = str.getLength();

        isize i = 0;

        // Skip leading whitespaces
        while (i < length && Characters::isWhitespace(str[i]))
        {
            i++;
        }

        if (i >= length)
        {
            return none;
        }

        bool isNegative = false;
        if (str[i] == '-')
        {
            isNegative = true;
            i++;
        }
        else if (str[i] == '+')
        {
            i++;
        }

        usize integerIndex;
        const auto integerPart = Internal::toInt<uint64>(str.subslice(i), true, integerIndex);
        if (!integerPart || integerIndex == 0)
        {
            return none;
        }

        FloatType result = static_cast<FloatType>(integerPart.getValue());
        i += integerIndex;

        if (i == length)
        {
            if (isNegative)
            {
                result = -result;
            }

            return result;
        }

        if (str[i] == '.')
        {
            i++;
            FloatType fraction = 0;
            FloatType place = 0.1;

            while (i < length && Characters::isDigit(str[i]))
            {
                fraction += Characters::toDigit(str[i]) * place;
                place *= 0.1;
                i++;
            }

            result += fraction;
        }

        if (i == length)
        {
            if (isNegative)
            {
                result = -result;
            }

            return result;
        }

        if (str[i] == 'e' || str[i] == 'E')
        {
            i++;
            bool isExpNegative = false;

            if (i < length && str[i] == '-')
            {
                isExpNegative = true;
                i++;
            }
            else if (i < length && str[i] == '+')
            {
                i++;
            }

            const auto exponentPart = Internal::toInt<uint64>(str.subslice(i), true, integerIndex);
            if (!exponentPart || integerIndex == 0)
            {
                return none;
            }

            int exponent = static_cast<int>(exponentPart.getValue());
            if (isExpNegative)
            {
                exponent = -exponent;
            }

            result *= static_cast<FloatType>(Internal::pow10(exponent));
        }

        if (isNegative)
        {
            result = -result;
        }

        return result;
    }

    template <Concepts::FloatingPoint FloatType>
    Option<String> fromFloat(FloatType value, int precision, bool useScientificNotation)
    {
        // Lambda for integer to string conversion
        const auto intToString = [](int64 value) -> String
        {
            if (value == 0)
            {
                return "0";
            }

            String result;
            while (value > 0)
            {
                result = static_cast<AnsiChar>('0' + value % 10) + result;
                value /= 10;
            }

            return result;
        };

        if (value != value) // NaN check
        {
            return String("NaN");
        }

        if (value == NumericLimits<FloatType>::infinity())
        {
            return String("inf");
        }

        if (value == -NumericLimits<FloatType>::infinity())
        {
            return String("-inf");
        }

        String result;
        bool isNegative = value < 0;
        int exponent = 0;

        if (isNegative)
        {
            value = -value;
        }

        if (value != 0)
        {
            // Freestanding base-10 exponent (no <cmath>).
            FloatType scaled = value;
            while (scaled >= FloatType(10))
            {
                scaled /= FloatType(10);
                ++exponent;
            }
            while (scaled > FloatType(0) && scaled < FloatType(1))
            {
                scaled *= FloatType(10);
                --exponent;
            }

            if (useScientificNotation || Internal::absInt(exponent) > precision)
            {
                value = scaled;
            }
            else
            {
                exponent = 0;
            }
        }

        // Handle the integer part
        int64 intPart = static_cast<int64>(value);
        FloatType fracPart = value - intPart;

        result = intToString(intPart);

        // Handle the fractional part
        if (precision > 0)
        {
            result += '.';

            for (int i = 0; i < precision; ++i)
            {
                fracPart *= 10;

                int digit = static_cast<int>(fracPart);
                result += static_cast<AnsiChar>('0' + digit);

                fracPart -= digit;
            }
        }

        // Remove trailing fractional zeros (and the decimal point) only.
        if (precision > 0)
        {
            while (!result.isEmpty() && result.getLast() == '0')
            {
                result.removeLast();
            }

            if (!result.isEmpty() && result.getLast() == '.')
            {
                result.removeLast();
            }
        }

        // Add exponent if necessary
        if (exponent != 0)
        {
            result += 'e';
            result += exponent < 0 ? "-" : "+";
            result += intToString(Internal::absInt(exponent));
        }

        if (isNegative)
        {
            result = '-' + result;
        }

        return result;
    }
}
