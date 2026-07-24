#pragma once

#include "Ark/Collections/Internal/RandomAccessIterator.hpp"
#include "Ark/Collections/Internal/ReverseIterator.hpp"
#include "Ark/Core/Comparison.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/Character.hpp"
#include "Ark/Strings/Strings.hpp"
#include "Ark/Strings/Utf8.hpp"
#include "Ark/Strings/Internal/Format.hpp"
#include "Ark/Strings/Utf8Strings.hpp"
#include <string_view>

namespace Ark
{
    /// A string slice class optimized for ANSI encoding.
    struct StringSlice final
    {
        friend struct String;

#pragma region Types

    public:
        /// The underlying character type.
        using CharType = char;

        /// The type of values stored in the string.
        using ValueType = CharType;

        /// The type of references to values in the string.
        using Reference = ValueType&;

        /// The type of const references to values in the string.
        using ConstReference = const ValueType&;

        /// The type of pointers to values in the string.
        using Pointer = ValueType*;

        /// The type of const pointers to values in the string.
        using ConstPointer = const ValueType*;

        /// The type used for sizes and indices.
        using SizeType = usize;

        /// The type used for pointer differences.
        using DifferenceType = isize;

    public:
        /// Invalid index constant.
        static constexpr SizeType InvalidIndex = static_cast<SizeType>(-1);

#pragma endregion

    private:
        CharType const* data{nullptr};
        SizeType length{0};

#pragma region Constructors and Assignments

    public:
        /// Default constructor.
        constexpr StringSlice() = default;

        /// Creates a StringSlice from a character pointer.
        /// @param str The characters to view.
        constexpr StringSlice(CharType const* str)
        {
            ARK_ASSERT(str != nullptr || length == 0);

            this->data = str;
            this->length = (str != nullptr) ? Utf8Strings::getByteLengthUnsafe(str) : 0;
        }

        /// Creates a StringSlice from a character pointer with known length.
        /// @param str The characters to view.
        /// @param length The length of the string.
        constexpr StringSlice(CharType const* str, SizeType length)
        {
            ARK_ASSERT(str != nullptr || length == 0);

            this->data = str;
            this->length = length;
        }

        /// Creates a String from a static character array with fixed length.
        /// @param str The characters to copy.
        /// @tparam N The size of the inline array.
        /// Constructs a string slice.
        template <usize N>
        constexpr StringSlice(CharType const (&str)[N])
        {
            this->data = str;
            this->length = Utf8Strings::getByteLength(str, N);
        }

        /// Constructor from String.
        /// @param str The String to create a slice from.
        /// @note Implementation is provided after String is fully defined.
        StringSlice(String& str);

        /// Constructor from const String.
        /// @param str The String to create a slice from.
        /// @note Implementation is provided after String is fully defined.
        StringSlice(String const& str);

        /// Copy constructor.
        constexpr StringSlice(StringSlice const&) = default;

        constexpr StringSlice& operator=(StringSlice const&) = default;

#pragma endregion

#pragma region Iterators

    public:
        /// The iterator for the string (const-only for immutable view).
        using Iterator = Collections::Internal::ConstRandomAccessIterator<StringSlice>;

        /// The const iterator for the string.
        using ConstIterator = Collections::Internal::ConstRandomAccessIterator<StringSlice>;

        /// The reverse iterator for the string.
        using ReverseIterator = Collections::Internal::ReverseIterator<Iterator>;

        /// The const reverse iterator for the string.
        using ConstReverseIterator = Collections::Internal::ReverseIterator<ConstIterator>;

        /// Gets an iterator to the beginning of the string.
        /// @return An iterator to the beginning of the string.
        constexpr Iterator getStartIterator()
        {
            return Iterator(data);
        }

        /// Gets a const iterator to the beginning of the string.
        /// @return A const iterator to the beginning of the string.
        constexpr ConstIterator getStartIterator() const
        {
            return ConstIterator(data);
        }

        /// Gets an iterator to the end of the string.
        /// @return An iterator to the end of the string.
        constexpr Iterator getEndIterator()
        {
            return Iterator(data + length);
        }

        /// Gets a const iterator to the end of the string.
        /// @return A const iterator to the end of the string.
        constexpr ConstIterator getEndIterator() const
        {
            return ConstIterator(data + length);
        }

        /// Gets a reverse iterator to the end of the string.
        /// @return A reverse iterator to the end of the string.
        constexpr ReverseIterator getStartReverseIterator()
        {
            return ReverseIterator(getEndIterator());
        }

        /// Gets a const reverse iterator to the end of the string.
        /// @return A const reverse iterator to the end of the string.
        constexpr ConstReverseIterator getStartReverseIterator() const
        {
            return ConstReverseIterator(getEndIterator());
        }

        /// Gets a reverse iterator to the beginning of the string.
        /// @return A reverse iterator to the beginning of the string.
        constexpr ReverseIterator getEndReverseIterator()
        {
            return ReverseIterator(getStartIterator());
        }

        /// Gets a const reverse iterator to the beginning of the string.
        /// @return A const reverse iterator to the beginning of the string.
        constexpr ConstReverseIterator getEndReverseIterator() const
        {
            return ConstReverseIterator(getStartIterator());
        }

#pragma endregion

#pragma region Basic Accessors

    public:
        constexpr CharType operator[](SizeType index) const
        {
            ARK_ASSERT(index < length);

            return data[index];
        }

        /// Get the length of the string.
        /// @return The number of characters in the string.
        constexpr SizeType getLength() const
        {
            return length;
        }

        /// Get the length of the string.
        /// @return The number of characters in the string.
        constexpr SizeType getCount() const
        {
            return length;
        }

        /// Get the size of the string, in bytes.
        /// @return The number of bytes in the string.
        constexpr SizeType getSize() const
        {
            return length * sizeof(CharType);
        }

        /// Check if the string is empty.
        /// @return `true` if the string is empty, otherwise `false`.
        constexpr bool isEmpty() const
        {
            return length == 0;
        }

        /// Get a pointer to the underlying data.
        /// @return A const pointer to the underlying data.
        constexpr CharType const* asPointer() const
        {
            if (length == 0)
            {
                return Utf8Strings::NullString;
            }

            return data;
        }

        // string_view-like aliases
        /// Returns a pointer to the underlying character data.
        constexpr CharType const* dataPtr() const
        {
            return asPointer();
        }
        /// Returns the element count.
        constexpr SizeType size() const
        {
            return length;
        }
        /// Returns the length.
        constexpr SizeType lengthValue() const
        {
            return length;
        }
        /// Returns whether the range is empty.
        constexpr bool empty() const
        {
            return isEmpty();
        }

        /// Gets a reference to the first element.
        /// @return A reference to the first element.
        constexpr ConstReference getFirst() const
        {
            ARK_ASSERT(!isEmpty());

            return data[0];
        }

        /// Gets a const reference to the first element.
        /// @return A const reference to the first element.
        /// Gets a reference to the last element.
        /// @return A reference to the last element.
        constexpr ConstReference getLast() const
        {
            ARK_ASSERT(!isEmpty());

            return data[length - 1];
        }

#pragma endregion

#pragma region Utilities

    public:
        /// Returns a sub-slice view.
        constexpr StringSlice slice(SizeType start, SizeType count = InvalidIndex) const
        {
            return substring(start, count);
        }

        /// Removes first.
        constexpr StringSlice removeFirst(SizeType k = 1) const
        {
            if (k >= length)
            {
                return {};
            }

            return substring(k);
        }

        /// Removes last.
        constexpr StringSlice removeLast(SizeType k = 1) const
        {
            if (k >= length)
            {
                return {};
            }

            return substring(0, length - k);
        }

#pragma endregion

#pragma region Element Accessors

        /// Returns a reference to the element at the given index, without bounds checking.
        /// @param index The index of the element to access.
        /// @return A reference to the element at the given index.
        /// Returns a const reference to the element at the given index, without bounds checking.
        /// @param index The index of the element to access.
        /// @return A const reference to the element at the given index.
        constexpr ConstReference get(SizeType index) const
        {
            ARK_ASSERT(index < length);

            return data[index];
        }

        /// Returns an Option containing a reference to the element at the given index.
        /// @param index The index of the element to access.
        /// @return An Option containing a reference to the element if the index is valid, None otherwise.
        constexpr Option<ValueType> tryGet(SizeType index) const
        {
            if (index < length)
            {
                return data[index];
            }

            return none;
        }

        // Removed toBytes; prefer constructing a Collections::Slice<uint8 const> at call site if needed.

#pragma endregion

#pragma region Comparisons

        /// Check if the string is equal to another.
        /// @param other The string to compare with.
        /// @return `true` if the strings are equal, otherwise `false`.
        bool isEqual(StringSlice const& other) const
        {
            if (length != other.length)
            {
                return false;
            }

            return Utf8Strings::compare(data, length, other.data, other.length) == Ordering::Equal;
        }

        /// Check if the string is equal to another, ignoring case.
        /// @param other The string to compare with.
        /// @return `true` if the strings are equal (ignoring case), otherwise `false`.
        bool isEqualIgnoreCase(StringSlice const& other) const
        {
            if (length != other.length)
            {
                return false;
            }

            return Utf8Strings::compareCaseInsensitive(data, other.data, length) == Ordering::Equal;
        }

        /// Compare lexicographically this string with another.
        /// @param other The string to compare with.
        /// @return The result of the comparison.
        Ordering compare(StringSlice const& other) const
        {
            return Utf8Strings::compare(data, length, other.data, other.length);
        }

        /// Compare this string with another, ignoring case.
        /// @param other The string to compare with.
        /// @return The result of the comparison.
        Ordering compareIgnoreCase(StringSlice const& other) const
        {
            Ordering result = Utf8Strings::compareCaseInsensitive(data, other.data, Ark::min(length, other.length));

            if (result != Ordering::Equal)
            {
                return result;
            }

            if (length == other.length)
            {
                return Ordering::Equal;
            }
            else
            {
                return (length < other.length) ? Ordering::Less
                                               : Ordering::Greater;
            }
        }

#pragma endregion

#pragma region Searches

        /// Find the first occurrence of a character.
        /// @param ch The character to find.
        /// @param start The index to start searching from.
        /// @return The index of the first occurrence, or InvalidIndex if not found.
        constexpr SizeType findFirst(CharType ch, SizeType start = 0) const
        {
            if (start >= length)
            {
                return InvalidIndex;
            }

            for (SizeType i = start; i < length; ++i)
            {
                if (data[i] == ch)
                {
                    return i;
                }
            }

            return InvalidIndex;
        }

        /// Find the first occurrence of a character (case insensitive).
        /// @param ch The character to find.
        /// @param start The index to start searching from.
        /// @return The index of the first occurrence, or InvalidIndex if not found.
        SizeType findFirstIgnoreCase(CharType ch, SizeType start = 0) const
        {
            if (start >= length)
            {
                return InvalidIndex;
            }

            const CharType lower = static_cast<CharType>(Character::toLowercase(ch));

            for (SizeType i = start; i < length; ++i)
            {
                if (Character::toLowercase(data[i]) == lower)
                {
                    return i;
                }
            }

            return InvalidIndex;
        }

        /// Find the first occurrence of a substring.
        /// @param needle The substring to find.
        /// @param start The index to start searching from.
        /// @return The index of the first occurrence, or InvalidIndex if not found.
        SizeType findFirst(StringSlice const& needle, SizeType start = 0) const;

        /// Find the last occurrence of a character.
        /// @param ch The character to find.
        /// @param end The index to stop searching at (exclusive).
        /// @return The index of the last occurrence, or InvalidIndex if not found.
        constexpr SizeType findLast(CharType ch, SizeType end = 0) const
        {
            if (end >= length)
            {
                return InvalidIndex;
            }

            for (SizeType i = length; i > end; --i)
            {
                if (data[i - 1] == ch)
                {
                    return i - 1;
                }
            }

            return InvalidIndex;
        }

        /// Find the last occurrence of a character (case insensitive).
        /// @param ch The character to find.
        /// @param end The index to stop searching at (exclusive).
        /// @return The index of the last occurrence, or InvalidIndex if not found.
        SizeType findLastIgnoreCase(CharType ch, SizeType end = 0) const
        {
            if (end >= length)
            {
                return InvalidIndex;
            }

            const CharType lower = static_cast<CharType>(Character::toLowercase(ch));

            for (SizeType i = length; i > end; --i)
            {
                if (Character::toLowercase(data[i - 1]) == lower)
                {
                    return i - 1;
                }
            }

            return InvalidIndex;
        }

        /// Find the last occurrence of a substring.
        /// @param needle The substring to find.
        /// @param end The index to stop searching at (exclusive).
        /// @return The index of the last occurrence, or InvalidIndex if not found.
        SizeType findLast(StringSlice const& needle, SizeType end = 0) const;

#pragma endregion

#pragma region Utilities and Conversion

        /// Create a sub-slice.
        /// @param start The start index of the sub-slice.
        /// @param end The end index of the sub-slice.
        /// @return A new StringSlice representing the sub-slice.
        constexpr StringSlice subslice(SizeType start, SizeType end) const
        {
            ARK_ASSERT(start <= end && end <= length);

            return StringSlice(data + start, end - start);
        }

        /// Create a sub-slice from a given start index to the end.
        /// @param start The start index of the sub-slice.
        /// @return A new StringSlice representing the sub-slice.
        constexpr StringSlice subslice(SizeType start) const
        {
            ARK_ASSERT(start <= length);

            return StringSlice(data + start, length - start);
        }

        /// Returns a substring view.
        constexpr StringSlice substring(SizeType position, SizeType count = InvalidIndex) const
        {
            if (position > length)
            {
                return {};
            }

            SizeType const maxCount = (count == InvalidIndex) ? (length - position) : Ark::min(count, length - position);
            return StringSlice(data + position, maxCount);
        }

        /// Remove leading and trailing whitespace.
        /// @return A StringSlice with whitespace removed from both ends.
        constexpr StringSlice trimmed() const
        {
            if (isEmpty())
            {
                return {};
            }

            SizeType start = 0;
            SizeType end = length;

            while (start < length && Character::isWhitespace(data[start]))
            {
                ++start;
            }

            while (end > start && Character::isWhitespace(data[end - 1]))
            {
                --end;
            }

            return substring(start, end - start);
        }

        /// Remove leading whitespace.
        /// @return A StringSlice with whitespace removed from the start.
        constexpr StringSlice trimmedStart() const
        {
            if (isEmpty())
            {
                return {};
            }

            SizeType start = 0;
            while (start < length && Character::isWhitespace(data[start]))
            {
                ++start;
            }

            return substring(start);
        }

        /// Remove trailing whitespace.
        /// @return A StringSlice with whitespace removed from the end.
        constexpr StringSlice trimmedEnd() const
        {
            if (isEmpty())
            {
                return {};
            }

            SizeType end = length;
            while (end > 0 && Character::isWhitespace(data[end - 1]))
            {
                --end;
            }

            return substring(0, end);
        }

        /// Check if the slice contains valid UTF-8 encoding.
        /// @return `true` if the slice contains valid UTF-8, otherwise `false`.
        bool isValidUtf8() const
        {
            return Utf8::isValid(*this);
        }

        String operator+(CharType const* other) const;

        String operator+(String const& other) const;

        String operator+(StringSlice const& other) const;

        /// Convert the slice to a String.
        /// @return A new String containing a copy of the slice's data.
        String toString() const;

#pragma endregion

#pragma region STL Compatibility API

    public:
        using value_type = ValueType;
        using size_type = SizeType;
        using difference_type = DifferenceType;
        using const_reference = ConstReference;
        using const_pointer = ConstPointer;
        using const_iterator = ConstIterator;

        constexpr const_reference front() const
        {
            return getFirst();
        }

        constexpr const_reference back() const
        {
            return getLast();
        }

        constexpr const_reference at(size_type index) const
        {
            return get(index);
        }

        // std::string_view-like iterator helpers
        constexpr ConstIterator begin() const
        {
            return getStartIterator();
        }

        constexpr ConstIterator cbegin() const
        {
            return getStartIterator();
        }

        constexpr ConstIterator end() const
        {
            return getEndIterator();
        }

        constexpr ConstIterator cend() const
        {
            return getEndIterator();
        }

        SizeType find(CharType ch, SizeType start = 0) const
        {
            return findFirst(ch, start);
        }

        SizeType find(StringSlice needle, SizeType start = 0) const
        {
            return findFirst(needle, start);
        }

        SizeType rfind(CharType ch, SizeType end = 0) const
        {
            return findLast(ch, end);
        }

        SizeType rfind(StringSlice needle, SizeType end = 0) const
        {
            return findLast(needle, end);
        }

        bool starts_with(StringSlice prefix) const
        {
            return Utf8Strings::startsWith(asPointer(), getLength(), prefix.asPointer(), prefix.getLength());
        }

        bool ends_with(StringSlice suffix) const
        {
            return Utf8Strings::endsWith(asPointer(), getLength(), suffix.asPointer(), suffix.getLength());
        }

        bool contains(StringSlice needle) const
        {
            return findFirst(needle) != InvalidIndex;
        }

        constexpr StringSlice substr(SizeType position, SizeType count = InvalidIndex) const
        {
            return substring(position, count);
        }

#pragma endregion
    };

    using AnsiStringSlice = StringSlice;
}

#include "Ark/Strings/String.hpp"

namespace Ark
{
    /// Constructs a string slice.
    inline StringSlice::StringSlice(String& str)
    {
        ARK_ASSERT(str.asPointer() != nullptr || str.getLength() == 0);

        this->data = str.asPointer();
        this->length = str.getLength();
    }

    /// Constructs a string slice.
    inline StringSlice::StringSlice(String const& str)
    {
        this->data = str.asPointer();
        this->length = str.getLength();
    }
}

namespace Ark
{
    inline String StringSlice::operator+(CharType const* other) const
    {
        String result;
        SizeType otherLength = Utf8Strings::getByteLengthUnsafe(other);

        result.reserve(getLength() + otherLength);
        result.append(*this);
        result.append(other, otherLength);

        return result;
    }

    inline String StringSlice::operator+(String const& other) const
    {
        String result;

        result.reserve(getLength() + other.getLength());
        result.append(*this);
        result.append(other.toSlice());

        return result;
    }

    inline String StringSlice::operator+(StringSlice const& other) const
    {
        String result;

        result.reserve(getLength() + other.getLength());
        result.append(*this);
        result.append(other);

        return result;
    }

    inline String operator+(StringSlice::CharType const* lhs, StringSlice const& rhs)
    {
        String result;
        StringSlice::SizeType lhsLength = Utf8Strings::getByteLengthUnsafe(lhs);

        result.reserve(lhsLength + rhs.getLength());
        result.append(lhs, lhsLength);
        result.append(rhs);

        return result;
    }

    /// Returns whether the text starts with the prefix.
    inline bool startsWith(StringSlice haystack, StringSlice prefix)
    {
        return Utf8Strings::startsWith(haystack.asPointer(), haystack.getLength(), prefix.asPointer(), prefix.getLength());
    }

    /// Returns whether the text ends with the suffix.
    inline bool endsWith(StringSlice haystack, StringSlice suffix)
    {
        return Utf8Strings::endsWith(haystack.asPointer(), haystack.getLength(), suffix.asPointer(), suffix.getLength());
    }

    /// Returns whether the element exists.
    inline bool contains(StringSlice haystack, StringSlice needle)
    {
        return Utf8Strings::contains(haystack.asPointer(), haystack.getLength(), needle.asPointer(), needle.getLength());
    }

    /// Finds first.
    inline Option<usize> findFirst(StringSlice haystack, StringSlice needle)
    {
        if (needle.isEmpty())
        {
            return static_cast<usize>(0);
        }
        char const* result = Utf8Strings::findFirst(haystack.asPointer(), haystack.getLength(), needle.asPointer(), needle.getLength());
        if (result == nullptr)
        {
            return none;
        }
        return static_cast<usize>(result - haystack.asPointer());
    }

    /// Finds last.
    inline Option<usize> findLast(StringSlice haystack, StringSlice needle)
    {
        if (needle.isEmpty())
        {
            return haystack.getCount();
        }
        char const* result = Utf8Strings::findLast(haystack.asPointer(), haystack.getLength(), needle.asPointer(), needle.getLength());
        if (result == nullptr)
        {
            return none;
        }
        return static_cast<usize>(result - haystack.asPointer());
    }

    /// Finds first.
    inline StringSlice::SizeType StringSlice::findFirst(StringSlice const& needle, SizeType start) const
    {
        SizeType const needleLength = needle.getLength();
        if (needleLength > length || start >= length)
        {
            return InvalidIndex;
        }

        if (needleLength == 0)
        {
            return start;
        }

        char const* result = Utf8Strings::findFirst(data + start, length - start, needle.asPointer(), needleLength);
        if (result == nullptr)
        {
            return InvalidIndex;
        }

        return static_cast<SizeType>(result - data);
    }

    /// Finds last.
    inline StringSlice::SizeType StringSlice::findLast(StringSlice const& needle, SizeType end) const
    {
        SizeType const needleLength = needle.getLength();
        if (needleLength > length || end >= length)
        {
            return InvalidIndex;
        }

        if (needleLength == 0)
        {
            return length;
        }

        // Search in the range [end, length)
        char const* result = Utf8Strings::findLast(data + end, length - end, needle.asPointer(), needleLength);
        if (result == nullptr)
        {
            return InvalidIndex;
        }

        return static_cast<SizeType>(result - data);
    }

    template <>
    struct Hasher<StringSlice>
    {
        usize operator()(StringSlice value) const
        {
            return computeHashArray(value.asPointer(), value.getCount());
        }
    };
}

namespace Ark::Utf8
{
    /// Iterate all codepoints, invoking callback(codepoint, byteIndex). Stops early if callback returns false.
    /// Iterates over each codepoint.
    template <typename Callback>
    void forEachCodepoint(StringSlice bytes, Callback&& callback)
    {
        usize index = 0;
        while (index < bytes.getCount())
        {
            auto const value = decodeAt(bytes, index);
            if (!value)
            {
                return;
            }

            UnicodeChar codepoint = value->first;
            usize nextIndex = value->second;
            if (!callback(codepoint, index))
            {
                return;
            }

            index = nextIndex;
        }
    }
}

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::StringSlice, char>
{
    template <typename FormatContext>
    auto format(Ark::StringSlice const& v, FormatContext& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::format_to(ctx.out(), "{}", std::string_view(v.asPointer(), v.getLength()));
    }

    constexpr auto parse(ARK_FORMAT_NAMESPACE::format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }
};

#include <functional>

namespace std
{
    template <>
    struct hash<Ark::StringSlice>
    {
        size_t operator()(Ark::StringSlice const& v) const
        {
            return static_cast<size_t>(Ark::computeHashArray(v.asPointer(), v.getCount()));
        }
    };
}
