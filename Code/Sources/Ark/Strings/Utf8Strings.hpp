#pragma once

#include "Ark/Core/Comparison.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/Strings.hpp"
// Note: Utf8.hpp is NOT included here to avoid circular dependency with StringSlice
// It will be included in the implementation file after StringSlice.hpp

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#define ARK_CONSTEXPR
#else
#define ARK_CONSTEXPR constexpr
#endif

namespace Ark::Utf8Strings
{
    static constexpr char NullCharacter = '\0';
    static constexpr char const* NullString = "";

    /// Get the byte length of a UTF-8 string (null-terminated).
    /// @param str The UTF-8 encoded string.
    /// @return The byte length of the string.
    /// @note The function does not check for buffer overflows. It only counts bytes until null.
    ARK_CONSTEXPR usize getByteLengthUnsafe(char const* str);

    /// Get the byte length of a UTF-8 string.
    /// @param str The UTF-8 encoded string.
    /// @param maxBytes The maximum number of bytes to count.
    /// @return The byte length of the string.
    /// @note The function stops counting at the null character or when maxBytes is reached.
    ARK_CONSTEXPR usize getByteLength(char const* str, usize maxBytes);

    /// Get the codepoint count of a UTF-8 string (null-terminated).
    /// @param str The UTF-8 encoded string.
    /// @return The number of Unicode codepoints in the string.
    /// @note The function validates UTF-8 encoding and does not check for buffer overflows.
    usize getLength(char const* str);

    /// Get the codepoint count of a UTF-8 string.
    /// @param str The UTF-8 encoded string.
    /// @param maxBytes The maximum number of bytes to scan.
    /// @return The number of Unicode codepoints in the string.
    /// @note The function validates UTF-8 encoding and stops counting at the null character or when maxBytes is reached.
    usize getLength(char const* str, usize maxBytes);

    /// Get the number of Unicode codepoints in a UTF-8 string.
    /// @param str The UTF-8 encoded string.
    /// @param byteLength The number of bytes in the string.
    /// @return The number of codepoints.
    usize getCodepointCount(char const* str, usize byteLength);

    /// Compare two UTF-8 strings codepoint-by-codepoint.
    /// @param lhs The left-hand UTF-8 encoded string.
    /// @param lhsByteLength The byte length of lhs.
    /// @param rhs The right-hand UTF-8 encoded string.
    /// @param rhsByteLength The byte length of rhs.
    /// @return The comparison result.
    Ordering compare(char const* lhs, usize lhsByteLength, char const* rhs, usize rhsByteLength);

    /// Compare two UTF-8 strings codepoint-by-codepoint (case insensitive).
    /// @param lhs The left-hand UTF-8 encoded string.
    /// @param lhsByteLength The byte length of lhs.
    /// @param rhs The right-hand UTF-8 encoded string.
    /// @param rhsByteLength The byte length of rhs.
    /// @return The comparison result.
    Ordering compareCaseInsensitive(char const* lhs, usize lhsByteLength, char const* rhs, usize rhsByteLength);

    /// Compare two UTF-8 strings codepoint-by-codepoint with known byte count.
    /// @param lhs The left-hand UTF-8 encoded string.
    /// @param rhs The right-hand UTF-8 encoded string.
    /// @param count The number of bytes to compare.
    /// @return The comparison result.
    /// @note The function validates UTF-8 encoding. The caller must ensure count does not exceed buffer bounds.
    Ordering compare(char const* lhs, char const* rhs, usize count);

    /// Compare two UTF-8 strings codepoint-by-codepoint (case insensitive) with known byte count.
    /// @param lhs The left-hand UTF-8 encoded string.
    /// @param rhs The right-hand UTF-8 encoded string.
    /// @param count The number of bytes to compare.
    /// @return The comparison result.
    /// @note The function validates UTF-8 encoding. The caller must ensure count does not exceed buffer bounds.
    Ordering compareCaseInsensitive(char const* lhs, char const* rhs, usize count);

    /// Find the first occurrence of a Unicode codepoint in a UTF-8 string.
    /// @param str The UTF-8 encoded string to search in.
    /// @param byteLength The byte length of str.
    /// @param codepoint The Unicode codepoint to find.
    /// @return Pointer to the start of the UTF-8 sequence containing the codepoint, or nullptr if not found.
    char const* findFirstCodepoint(char const* str, usize byteLength, UnicodeChar codepoint);

    /// Find the last occurrence of a Unicode codepoint in a UTF-8 string.
    /// @param str The UTF-8 encoded string to search in.
    /// @param byteLength The byte length of str.
    /// @param codepoint The Unicode codepoint to find.
    /// @return Pointer to the start of the UTF-8 sequence containing the codepoint, or nullptr if not found.
    char const* findLastCodepoint(char const* str, usize byteLength, UnicodeChar codepoint);

    /// Find the first occurrence of a UTF-8 substring (codepoint-aware).
    /// @param haystack The UTF-8 encoded string to search in.
    /// @param haystackByteLength The byte length of haystack.
    /// @param needle The UTF-8 encoded substring to find.
    /// @param needleByteLength The byte length of needle.
    /// @return Pointer to the start of the match, or nullptr if not found.
    char const* findFirst(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength);

    /// Find the last occurrence of a UTF-8 substring (codepoint-aware).
    /// @param haystack The UTF-8 encoded string to search in.
    /// @param haystackByteLength The byte length of haystack.
    /// @param needle The UTF-8 encoded substring to find.
    /// @param needleByteLength The byte length of needle.
    /// @return Pointer to the start of the match, or nullptr if not found.
    char const* findLast(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength);

    /// Check if a UTF-8 string starts with a prefix (codepoint-aware).
    /// @param str The UTF-8 encoded string.
    /// @param strByteLength The byte length of str.
    /// @param prefix The UTF-8 encoded prefix.
    /// @param prefixByteLength The byte length of prefix.
    /// @return `true` if str starts with prefix, otherwise `false`.
    bool startsWith(char const* str, usize strByteLength, char const* prefix, usize prefixByteLength);

    /// Check if a UTF-8 string ends with a suffix (codepoint-aware).
    /// @param str The UTF-8 encoded string.
    /// @param strByteLength The byte length of str.
    /// @param suffix The UTF-8 encoded suffix.
    /// @param suffixByteLength The byte length of suffix.
    /// @return `true` if str ends with suffix, otherwise `false`.
    bool endsWith(char const* str, usize strByteLength, char const* suffix, usize suffixByteLength);

    /// Check if a UTF-8 string contains a substring (codepoint-aware).
    /// @param haystack The UTF-8 encoded string to search in.
    /// @param haystackByteLength The byte length of haystack.
    /// @param needle The UTF-8 encoded substring to find.
    /// @param needleByteLength The byte length of needle.
    /// @return `true` if needle is found in haystack, otherwise `false`.
    bool contains(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength);

    /// Check if a UTF-8 string contains a substring (codepoint-aware, case insensitive).
    /// @param haystack The UTF-8 encoded string to search in.
    /// @param haystackByteLength The byte length of haystack.
    /// @param needle The UTF-8 encoded substring to find.
    /// @param needleByteLength The byte length of needle.
    /// @return `true` if needle is found in haystack (ignoring case), otherwise `false`.
    bool containsIgnoreCase(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength);
}

#include "Ark/Strings/Platform/Generic/Utf8Strings.hpp"

#undef ARK_CONSTEXPR
