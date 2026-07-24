#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Collections/Internal/RandomAccessIterator.hpp"
#include "Ark/Collections/Slice.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Functions.hpp"
#include "Ark/Strings/Character.hpp"
#include "Ark/Strings/Internal/Format.hpp"
#include "Ark/Strings/Strings.hpp"
#include "Ark/Strings/Utf8.hpp"
#include "Ark/Strings/Utf8Strings.hpp"

namespace Ark
{
    /// @name String Base
    /// Base class for string operations, optimized for ANSI encoding.
    struct String final
    {
    public:
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

#pragma endregion

    public:
        static constexpr SizeType InvalidIndex = static_cast<SizeType>(-1);

    private:
        Memory::Allocator& allocator{Memory::getDefaultAllocator()};
        CharType* data{nullptr};
        SizeType length{0};
        SizeType capacity{1};

    public:
#pragma region Constructors and Assignments

        String()
            : allocator{Memory::getDefaultAllocator()}
            , data{nullptr}
            , length{0}
            , capacity{1}
        {
            data = allocator.allocateArray<CharType>(capacity);
            data[0] = Utf8Strings::NullCharacter;
        }

        String(SizeType initialCapacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , data{nullptr}
            , length{0}
            , capacity{Ark::max(initialCapacity, SizeType{1})}
        {
            data = allocator.allocateArray<CharType>(capacity);
            data[0] = Utf8Strings::NullCharacter;
        }

        /// Creates a String from a character pointer.
        /// @param str The characters to copy.
        String(CharType const* str, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
            length = (str != nullptr) ? Utf8Strings::getByteLengthUnsafe(str) : 0;
            capacity = length + 1;
            data = allocator.allocateArray<CharType>(capacity);

            if (length > 0)
            {
                Memory::copy(str, data, length * sizeof(CharType));
            }

            *(data + length) = Utf8Strings::NullCharacter;

            ARK_ASSERT(Utf8::isValid(data, length));
        }

        /// Creates a String from a character pointer with known length.
        /// @param str The characters to copy.
        /// @param length The length of the string.
        String(const CharType* str, SizeType length, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
            capacity = length + 1;
            data = allocator.allocateArray<CharType>(capacity);

            if (length > 0)
            {
                Memory::copy(str, data, length * sizeof(CharType));
            }

            this->length = length;
            *(data + length) = Utf8Strings::NullCharacter;

            ARK_ASSERT(Utf8::isValid(data, length));
        }

        /// Creates a String filled with a specific character.
        /// @param allocator The allocator to use.
        /// @param ch The character to fill the string with.
        /// @param length The length of the string.
        String(CharType ch, SizeType length, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
            capacity = length + 1;

            data = allocator.allocateArray<CharType>(capacity);

            Memory::set(data, length, ch);
            data[length] = Utf8Strings::NullCharacter;
            this->length = length;

            ARK_ASSERT(Utf8::isValid(data, length));
        }

        /// Creates a String from a string slice.
        /// @param str The string slice to copy.
        String(StringSlice const& str, Memory::Allocator& allocator = Memory::getDefaultAllocator());

        /// Creates a String from a Slice of characters.
        /// @param characters The characters to copy.
        String(const Collections::Slice<CharType> characters, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , data{nullptr}
            , length{characters.getCount()}
            , capacity{length + 1}
        {
            data = allocator.allocateArray<CharType>(capacity);

            if (length > 0)
            {
                Memory::copy(characters.asPointer(), data, length * sizeof(CharType));
            }

            data[length] = Utf8Strings::NullCharacter;

            ARK_ASSERT(Utf8::isValid(data, length));
        }

        /// Creates a String from a static character array.
        /// @param str The characters to copy.
        /// @tparam N The size of the inline array.
        template <usize N>
        String(CharType const (&str)[N], Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , data{nullptr}
            , length{Utf8Strings::getByteLengthUnsafe(str)}
            , capacity{length + 1}
        {
            data = allocator.allocateArray<CharType>(capacity);

            if (length > 0)
            {
                Memory::copy(str, data, length * sizeof(CharType));
            }

            data[length] = Utf8Strings::NullCharacter;

            ARK_ASSERT(Utf8::isValid(data, length));
        }

        /// Constructs a string.
        String(const String& other)
            : allocator{other.allocator}
            , data{nullptr}
            , length{other.length}
            , capacity{Ark::max(other.capacity, SizeType{1})}
        {
            data = allocator.allocateArray<CharType>(capacity);

            if (other.data != nullptr && length > 0)
            {
                Memory::copy(other.data, data, length * sizeof(CharType));
            }

            data[length] = Utf8Strings::NullCharacter;

            ARK_ASSERT(Utf8::isValid(data, length));
        }

        String(String&& other)
            : allocator{other.allocator}
            , data{other.data}
            , length{other.length}
            , capacity{other.capacity}
        {
            ARK_ASSERT(data == nullptr || Utf8::isValid(data, length));
            other.data = nullptr;
            other.length = 0;
            other.capacity = 0;
        }

        ~String()
        {
            if (data != nullptr)
            {
                allocator.deallocate(const_cast<CharType*>(data));

                data = nullptr;
                capacity = 0;
                length = 0;
            }
        }

        String& operator=(String const& other)
        {
            if (this != &other)
            {
                if (capacity < other.length + 1 || data == nullptr)
                {
                    if (data != nullptr)
                    {
                        allocator.deallocate(const_cast<CharType*>(data));
                    }

                    capacity = Ark::max(capacity, other.length + 1);
                    data = allocator.allocateArray<CharType>(capacity);
                }

                length = other.length;

                if (other.data != nullptr)
                {
                    Memory::copy(other.data, data, (length + 1) * sizeof(CharType));
                }
                else
                {
                    data[0] = Utf8Strings::NullCharacter;
                }
            }

            return *this;
        }

        String& operator=(String&& other)
        {
            if (this == &other)
            {
                return *this;
            }

            if (&allocator == &other.allocator)
            {
                if (data != nullptr)
                {
                    allocator.deallocate(const_cast<CharType*>(data));
                }

                capacity = other.capacity;
                length = other.length;
                data = other.data;

                other.capacity = 0;
                other.length = 0;
                other.data = nullptr;
            }
            else
            {
                *this = other;
                other.clear();
            }

            return *this;
        }

#pragma endregion

#pragma region Iterators

    public:
        /// The iterator for the string.
        using Iterator = Collections::Internal::RandomAccessIterator<String>;

        /// The const iterator for the string.
        using ConstIterator = Collections::Internal::ConstRandomAccessIterator<String>;

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

#pragma region Accessors

    public:
        /// Get the capacity of the string.
        /// @return The current capacity of the string.
        constexpr SizeType getCapacity() const
        {
            return capacity;
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

        /// Get a pointer to the underlying data.
        /// @return A pointer to the underlying data.
        constexpr CharType* asPointer()
        {
            return data;
        }

        /// Returns a reference to the element at the given index, without bounds checking.
        /// @param index The index of the element to access.
        /// @return A reference to the element at the given index.
        constexpr Reference get(SizeType index)
        {
            ARK_ASSERT(index < length);

            return data[index];
        }

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
        constexpr Option<ValueType> tryGet(SizeType index)
        {
            if (index < length)
            {
                return data[index];
            }

            return none;
        }

        /// Returns an Option containing a const reference to the element at the given index.
        /// @param index The index of the element to access.
        /// @return An Option containing a const reference to the element if the index is valid, None otherwise.
        constexpr Option<ValueType> tryGet(SizeType index) const
        {
            if (index < length)
            {
                return data[index];
            }

            return none;
        }

        /// Sets a character at a specific index.
        /// @param index The index of the character to set.
        /// @param value The character to set.
        void set(SizeType index, CharType value)
        {
            ARK_ASSERT(index < length);

            data[index] = value;
        }

        /// Gets a reference to the first element.
        /// @return A reference to the first element.
        constexpr Reference getFirst()
        {
            ARK_ASSERT(!isEmpty());

            return data[0];
        }

        /// Gets a const reference to the first element.
        /// @return A const reference to the first element.
        constexpr ConstReference getFirst() const
        {
            ARK_ASSERT(!isEmpty());

            return data[0];
        }

        /// Gets a reference to the last element.
        /// @return A reference to the last element.
        constexpr Reference getLast()
        {
            ARK_ASSERT(!isEmpty());

            return data[length - 1];
        }

        /// Gets a const reference to the last element.
        /// @return A const reference to the last element.
        constexpr ConstReference getLast() const
        {
            ARK_ASSERT(!isEmpty());

            return data[length - 1];
        }

#pragma endregion

#pragma region Searches

        /// Returns whether the element exists.
        bool containsIgnoreCase(CharType ch) const
        {
            return findFirstIgnoreCase(ch) != InvalidIndex;
        }

        /// Check if the string contains a StringSlice.
        /// @param slice The StringSlice to search for.
        /// @return `true` if the StringSlice is found, `false` otherwise.
        bool contains(const StringSlice& slice) const;

        /// Check if the string contains a StringSlice (case insensitive).
        /// @param slice The StringSlice to search for.
        /// @return true if the StringSlice is found (ignoring case); otherwise, `false`.
        bool containsIgnoreCase(const StringSlice& slice) const;

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

        /// Finds first ignore case.
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
        /// @param pattern The pattern to find.
        /// @param start The index to start searching from.
        /// @return The index of the first occurrence, or InvalidIndex if not found.
        SizeType findFirst(const StringSlice& pattern, SizeType start = 0) const;

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

        /// Finds last ignore case.
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
        /// @param pattern The pattern to find.
        /// @param end The index to stop searching at (exclusive).
        /// @return The index of the last occurrence, or InvalidIndex if not found.
        SizeType findLast(const StringSlice& pattern, SizeType end = 0) const;

        /// Check if the string starts with a given prefix.
        /// @param prefix The prefix to check for.
        /// @return `true` if the string starts with the prefix, otherwise `false`.
        bool startsWith(const StringSlice& prefix) const;

        /// Check if the string ends with a given suffix.
        /// @param suffix The suffix to check for.
        /// @return `true` if the string ends with the suffix, otherwise `false`.
        bool endsWith(const StringSlice& suffix) const;

#pragma endregion

#pragma region Mutating Utilities

        /// Formats a value as text.
        template <typename... Args>
        static String format(FormatString<Args...> format, Args&&... args)
        {
            auto const formatted = Internal::formatToText(format, Ark::forward<Args>(args)...);
            return String(formatted.data(), static_cast<usize>(formatted.size()));
        }

        // UTF-8 helpers and codepoint-aware APIs
        bool isValidUtf8() const;

        SizeType getCharCount() const;

        bool isCharBoundary(SizeType index) const;

        Result<StringSlice, String> substringChars(SizeType startChar, SizeType charCount = InvalidIndex) const;

        Option<SizeType> findFirstCodepoint(UnicodeChar codepoint) const;

        void appendCodepoint(UnicodeChar codepoint);

        /// Get a substring of the string.
        /// @param start The start index of the substring.
        /// @param count The number of characters to include (or InvalidIndex for rest of string).
        /// @return A StringSlice representing the substring.
        StringSlice substring(SizeType start, SizeType count = InvalidIndex) const;

        /// Get a substring of the first n characters.
        StringSlice prefix(SizeType maxLength) const;

        /// Get a substring of the last n characters.
        StringSlice suffix(SizeType maxLength) const;

        /// Return a substring removing the first k characters.
        StringSlice dropFirst(SizeType k = 1) const;

        /// Return a substring removing the last k characters.
        StringSlice dropLast(SizeType k = 1) const;

        /// Split the string into parts based on a delimiter.
        /// @param delimiter The delimiter to split on.
        /// @return A vector of StringSlices, each containing a part of the split string.
        Collections::Array<StringSlice> split(CharType delimiter) const;

        /// Join strings.
        /// @param strings The strings to join.
        /// @return A new String containing the joined strings.
        static String join(const Collections::Slice<String*> strings, String const& separator = {});

        /// Join strings with this string as a separator.
        /// @param strings The strings to join.
        /// @return A new String containing the joined strings.
        String join(const Collections::Slice<String*> strings);

        /// Remove leading and trailing whitespace.
        /// @return A StringSlice with whitespace removed from both ends.
        StringSlice trimmed() const;

        /// Remove leading whitespace.
        /// @return A StringSlice with whitespace removed from the start.
        StringSlice trimmedStart() const;

        /// Remove trailing whitespace.
        /// @return A StringSlice with whitespace removed from the end.
        StringSlice trimmedEnd() const;

        /// Remove a prefix from the string if it exists.
        /// @param prefix The prefix to remove.
        /// @return A StringSlice with the prefix removed.
        StringSlice strippedPrefix(String const& prefix);

        /// Remove a suffix from the string if it exists.
        /// @param prefix The suffix to remove.
        /// @return A StringSlice with the suffix removed.
        StringSlice strippedSuffix(String const& suffix);

        /// Convert the string to lowercase.
        /// @return A new String with all characters converted to lowercase.
        String toLowercase() const;

        /// Convert the string to uppercase.
        /// @return A new String with all characters converted to uppercase.
        String toUppercase() const;

#pragma endregion

#pragma region Convenience Modifiers

        /// Reserves space for a certain number of characters.
        /// @param additionalCapacity The additional capacity to reserve.
        /// @details This function will reserve at least the additional capacity.
        void reserve(SizeType additionalCapacity)
        {
            if (capacity < length + additionalCapacity)
            {
                grow(length + additionalCapacity);
            }
        }

        /// Reserves space for a certain number of characters.
        /// @param additionalCapacity The additional capacity to reserve.
        /// @details This function will reserve exactly the additional capacity.
        void reserveExact(SizeType additionalCapacity)
        {
            if (capacity < length + additionalCapacity)
            {
                growExact(length + additionalCapacity);
            }
        }

        /// Resizes the string to a specific length.
        /// @param newLength The new length of the string.
        /// @param ch The character to fill new spaces with if expanding.
        void resize(SizeType newLength, CharType ch = '\0')
        {
            if (newLength + 1 > capacity)
            {
                grow(newLength + 1);
            }

            if (newLength > length)
            {
                memset(data + length, ch, newLength - length);
            }

            length = newLength;
            data[length] = '\0';
        }

        /// Shrinks the capacity to fit the current length.
        void shrinkToFit()
        {
            if (capacity > length + 1)
            {
                growExact(length + 1);
            }
        }

        /// Clears the contents of the string.
        void clear()
        {
            length = 0;
            data[0] = '\0';
        }

        /// Assign from another String.
        /// @param other The source string.
        String& assign(String const& other);

        /// Assign by moving from another String.
        /// @param other The source string to move from.
        String& assign(String&& other);

        /// Assign from a null-terminated C-string.
        /// @param str The C-string to copy (can be nullptr to clear).
        String& assign(CharType const* str);

        /// Assign from a character array with explicit length.
        /// @param str The character array to copy.
        /// @param count The number of characters to copy.
        String& assign(CharType const* str, SizeType count);

        /// Assign with a repeated character.
        /// @param count The number of repetitions.
        /// @param ch The character to repeat.
        String& assign(SizeType count, CharType ch);

        /// Assign from a StringSlice.
        /// @param slice The slice to copy.
        String& assign(StringSlice const& slice);

        /// Assign from a Slice of characters.
        /// @param characters The characters to copy.
        String& assign(Collections::Slice<CharType> characters);

        /// Assign a substring of another String.
        /// @param other The source string.
        /// @param position The starting position in the source string.
        /// @param count The number of characters to copy (InvalidIndex to copy to end).
        String& assign(String const& other, SizeType position, SizeType count = InvalidIndex);

        /// Appends a character to the string.
        /// @param ch The character to append.
        void append(CharType ch)
        {
            if (length + 1 >= capacity)
            {
                grow();
            }

            data[length] = ch;
            length++;

            data[length] = '\0';
            ARK_ASSERT(Utf8::isValid(data, length));
        }

        /// Appends a character array to this string.
        /// @param slice The string slice to append.
        void append(const CharType* characters, SizeType count);

        /// Appends a character array to this string.
        /// @param slice The string slice to append.
        void push(const CharType* characters, SizeType count);

        /// Appends a string slice to this string.
        /// @param slice The string slice to append.
        void append(const StringSlice& slice);

        /// Formats and appends text.
        template <typename... Args>
        void appendFormat(FormatString<Args...> format, Args&&... args)
        {
            auto const formatted = Internal::formatToText(format, Ark::forward<Args>(args)...);
            append(formatted.data(), static_cast<usize>(formatted.size()));
        }

        /// Appends a string slice to this string.
        /// @param slice The string slice to append.
        void push(const StringSlice& slice)
        {
            append(slice);
        }

        /// Appends a character to the string.
        /// @param ch The character to append.
        void push(CharType ch)
        {
            append(ch);
        }

        /// Removes the last character from the string.
        void removeLast()
        {
            ARK_ASSERT(length > 0);

            if (length > 0)
            {
                --length;
                data[length] = '\0';
            }
        }

        /// Removes the last character from the string.
        void pop()
        {
            removeLast();
        }

        /// Inserts a character at a specific index.
        /// @param index The index at which to insert.
        /// @param ch The character slice to insert.
        void insert(SizeType index, String::CharType ch);

        /// Inserts a string slice at a specific index.
        /// @param index The index at which to insert.
        /// @param slice The string slice to insert.
        void insert(SizeType index, const StringSlice& slice);

        /// Removes a range of characters from the string.
        /// @param start The start index of the range to remove.
        /// @param end The end index of the range to remove.
        void remove(SizeType start, SizeType end);

        /// Remove a range of characters and return them as a new String.
        /// @param start The start index of the range to remove.
        /// @param end The end index of the range to remove.
        /// @return A new String containing the removed characters.
        String drain(SizeType start, SizeType end);

        /// Removes all occurrences of a character from the string.
        /// @param ch The character to remove.
        /// @return The number of characters removed.
        SizeType removeAll(CharType ch);

        /// Removes all occurrences of a string slice from the string.
        /// @param slice The string slice to remove.
        /// @return The number of characters removed.
        SizeType removeAll(const StringSlice& slice);

        /// Removes a character at a specific index.
        /// @param index The index of the character to remove.
        void removeAt(SizeType index);

        /// Remove a prefix from the string if it exists.
        /// @param prefix The prefix to remove.
        /// @return `true` if the prefix was removed, otherwise `false`.
        bool stripPrefix(String const& prefix);

        /// Remove a suffix from the string if it exists.
        /// @param suffix The suffix to remove.
        /// @return `true` if the suffix was removed, otherwise `false`.
        bool stripSuffix(String const& suffix);

#pragma endregion

#pragma region Utilities

        /// Replaces a range of characters with a string slice.
        /// @param start The start index of the range to replace.
        /// @param end The end index of the range to replace.
        /// @param slice The string slice to insert.
        void replace(SizeType start, SizeType end, const StringSlice& slice);

        /// Replace all occurrences of a substring with another string.
        /// @param from The substring to replace.
        /// @param to The string to replace with.
        /// @return A new String with all occurrences replaced.
        SizeType replace(const StringSlice& from, const StringSlice& to);

        /// Split the string into two at the given index.
        /// @param at The index at which to split the string.
        /// @return A new String containing the second part of the split.
        String splitOff(SizeType at)
        {
            ARK_ASSERT(at <= length);
            String result(asPointer() + at, length - at, allocator);
            resize(at);
            return result;
        }

        /// Truncates the string to a specified length.
        /// @param newLength The new length of the string.
        void truncate(SizeType newLength)
        {
            if (newLength < length)
            {
                length = newLength;
                data[length] = '\0';
            }
        }

        /// Removes whitespace from the start and end of the string.
        void trim()
        {
            SizeType start = 0;
            while (start < length && Character::isWhitespace(data[start]))
            {
                ++start;
            }

            SizeType end = length;
            while (end > start && Character::isWhitespace(data[end - 1]))
            {
                --end;
            }

            if (start > 0 || end < length)
            {
                memmove(data, data + start, end - start);

                length = end - start;
                data[length] = '\0';
            }
        }

        /// Removes whitespace from the start of the string.
        void trimStart()
        {
            SizeType start = 0;
            while (start < length && Character::isWhitespace(data[start]))
            {
                ++start;
            }

            if (start > 0)
            {
                memmove(data, data + start, length - start);

                length = length - start;
                data[length] = '\0';
            }
        }

        /// Removes whitespace from the end of the string.
        void trimEnd()
        {
            SizeType end = length;
            while (end > 0 && Character::isWhitespace(data[end - 1]))
            {
                --end;
            }

            if (end < length)
            {
                length = end;
                data[length] = '\0';
            }
        }

#pragma endregion

#pragma region Comparisons

        /// Check if the string is equal to another.
        /// @param other The string to compare with.
        /// @return `true` if the strings are equal, otherwise `false`.
        bool isEqual(String const& other) const
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
        bool isEqualIgnoreCase(String const& other) const
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
        Ordering compare(String const& other) const
        {
            AnsiChar const* lhsPtr = asPointer();
            AnsiChar const* rhsPtr = other.asPointer();
            Ordering const result = Utf8Strings::compare(lhsPtr, rhsPtr, Ark::min(length, other.length));
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
                return (length < other.length) ? Ordering::Less : Ordering::Greater;
            }
        }

        /// Compare this string with another, ignoring case.
        /// @param other The string to compare with.
        /// @return The result of the comparison.
        Ordering compareIgnoreCase(String const& other) const
        {
            AnsiChar const* lhsPtr = asPointer();
            AnsiChar const* rhsPtr = other.asPointer();
            Ordering result = Utf8Strings::compareCaseInsensitive(lhsPtr, rhsPtr, Ark::min(length, other.length));

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

#pragma region Conversion

        StringSlice const toSlice() const;

        StringSlice toSlice();

#pragma endregion

#pragma region Operators

        explicit operator CharType const*() const
        {
            return data;
        }

        explicit operator CharType*()
        {
            return data;
        }

        constexpr CharType operator[](SizeType index) const
        {
            ARK_ASSERT(index < length);

            return data[index];
        }

        constexpr CharType& operator[](SizeType index)
        {
            ARK_ASSERT(index < length);

            return data[index];
        }

        bool operator==(String const& other) const
        {
            return compare(other) == Ordering::Equal;
        }

        bool operator!=(String const& other) const
        {
            return !(*this == other);
        }

        bool operator<(String const& other) const
        {
            return compare(other) == Ordering::Less;
        }

        bool operator<=(String const& other) const
        {
            return static_cast<int>(compare(other)) <= 0;
        }

        bool operator>(String const& other) const
        {
            return compare(other) == Ordering::Greater;
        }

        bool operator>=(String const& other) const
        {
            return static_cast<int>(compare(other)) >= 0;
        }

        bool operator==(CharType const* other) const
        {
            return compare(other) == Ordering::Equal;
        }

        bool operator!=(CharType const* other) const
        {
            return !(*this == other);
        }

        bool operator<(CharType const* other) const
        {
            return compare(other) == Ordering::Less;
        }

        bool operator<=(CharType const* other) const
        {
            Ordering result = compare(other);
            return result == Ordering::Less || result == Ordering::Equal;
        }

        bool operator>(CharType const* other) const
        {
            return compare(other) == Ordering::Greater;
        }

        bool operator>=(CharType const* other) const
        {
            Ordering result = compare(other);
            return result == Ordering::Greater || result == Ordering::Equal;
        }

        String operator+(String const& other) const;

        String operator+(CharType const* other) const;

        String operator+(CharType ch) const;

        String& operator+=(String const& other);

        String& operator+=(const CharType* other);

        String& operator+=(CharType ch);

#pragma endregion

#pragma region Modifiers

    public:
        /// Inserts at.
        void insertAt(SizeType index, CharType ch)
        {
            insert(index, ch);
        }

        /// Inserts at.
        void insertAt(SizeType index, const StringSlice& slice)
        {
            insert(index, slice);
        }

        /// Removes first.
        void removeFirst()
        {
            ARK_ASSERT(!isEmpty());
            removeAt(0);
        }

        /// Removes subrange.
        void removeSubrange(SizeType start, SizeType end)
        {
            remove(start, end);
        }

        /// Replaces a subrange of elements.
        void replaceSubrange(SizeType start, SizeType end, StringSlice const& slice)
        {
            replace(start, end, slice);
        }

#pragma endregion

#pragma region Private

    private:
        /// Increases capacity.
        void grow(SizeType newCapacity)
        {
            newCapacity = Ark::max(newCapacity, capacity * 2);
            growExact(newCapacity);
        }

        /// Increases capacity.
        void grow()
        {
            growExact((capacity == 0) ? 1 : capacity * 2);
        }

        /// Increases capacity to an exact size.
        void growExact(SizeType newCapacity)
        {
            CharType* newData = allocator.allocateArray<CharType>(newCapacity);

            if (data != nullptr && length > 0)
            {
                Memory::copy(data, newData, length * sizeof(CharType));
            }

            newData[length] = Utf8Strings::NullCharacter;

            if (data != nullptr)
            {
                allocator.deallocate(const_cast<CharType*>(data));
            }

            data = newData;
            capacity = newCapacity;
        }

#pragma endregion

#pragma region STL Compatibility API

    public:
        using value_type = ValueType;
        using size_type = SizeType;
        using difference_type = DifferenceType;
        using reference = Reference;
        using const_reference = ConstReference;
        using pointer = Pointer;
        using const_pointer = ConstPointer;
        using iterator = Iterator;
        using const_iterator = ConstIterator;

        constexpr Iterator begin()
        {
            return getStartIterator();
        }

        constexpr ConstIterator begin() const
        {
            return getStartIterator();
        }

        constexpr ConstIterator cbegin() const
        {
            return getStartIterator();
        }

        constexpr Iterator end()
        {
            return getEndIterator();
        }

        constexpr ConstIterator end() const
        {
            return getEndIterator();
        }

        constexpr size_type size() const
        {
            return getLength();
        }

        constexpr bool empty() const
        {
            return isEmpty();
        }

        constexpr const_pointer c_str() const
        {
            return asPointer();
        }

        constexpr reference front()
        {
            return getFirst();
        }

        constexpr const_reference front() const
        {
            return getFirst();
        }

        constexpr reference back()
        {
            return getLast();
        }

        constexpr const_reference back() const
        {
            return getLast();
        }

        constexpr reference at(size_type index)
        {
            return get(index);
        }

        constexpr const_reference at(size_type index) const
        {
            return get(index);
        }

        void push_back(CharType ch)
        {
            append(ch);
        }

        void pop_back()
        {
            removeLast();
        }

        size_type find(CharType ch, size_type start = 0) const
        {
            return findFirst(ch, start);
        }

        size_type find(StringSlice const& pattern, size_type start = 0) const
        {
            return findFirst(pattern, start);
        }

        size_type rfind(CharType ch, size_type end = 0) const
        {
            return findLast(ch, end);
        }

        size_type rfind(StringSlice const& pattern, size_type end = 0) const
        {
            return findLast(pattern, end);
        }

        bool starts_with(StringSlice const& prefix) const
        {
            return startsWith(prefix);
        }

        bool ends_with(StringSlice const& suffix) const
        {
            return endsWith(suffix);
        }

        String substr(size_type position, size_type count = InvalidIndex) const
        {
            if (position >= length)
            {
                return String();
            }

            SizeType const available = length - position;
            SizeType const clampedCount = (count == InvalidIndex) ? available : Ark::min(count, available);

            return String(data + position, clampedCount);
        }

        constexpr ConstIterator cend() const
        {
            return getEndIterator();
        }

#pragma endregion
    };

    String operator+(String::CharType const* lhs, String const& rhs);

    String operator+(String::CharType lhs, String const& rhs);

    /// Find the first occurrence of a substring in a String (free function).
    /// @param haystack The string to search in.
    /// @param needle The substring to find.
    /// @return The index of the first occurrence, or String::InvalidIndex if not found.
    String::SizeType findFirst(String const& haystack, StringSlice const& needle);

    /// Find the last occurrence of a substring in a String (free function).
    /// @param haystack The string to search in.
    /// @param needle The substring to find.
    /// @return The index of the last occurrence, or String::InvalidIndex if not found.
    String::SizeType findLast(String const& haystack, StringSlice const& needle);

    using AnsiString = String;
}

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::String, char>
{
    template <typename FormatContext>
    auto format(Ark::String const& v, FormatContext& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::format_to(ctx.out(), "{}", std::string_view(v.asPointer(), v.getLength()));
    }

    constexpr auto parse(ARK_FORMAT_NAMESPACE::format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }
};

namespace std
{
    template <>
    struct hash<Ark::String>
    {
        size_t operator()(Ark::String const& v) const
        {
            return static_cast<size_t>(Ark::computeHashArray(v.asPointer(), v.getCount()));
        }
    };
}
