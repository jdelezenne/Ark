#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/Strings/Utf8.hpp"
#include "Ark/Strings/Utf8Strings.hpp"

namespace Ark
{
    String::String(StringSlice const& str, Memory::Allocator& allocator)
        : allocator{allocator}
    {
        length = str.getLength();
        capacity = length + 1;

        data = allocator.allocateArray<CharType>(capacity);
        if (length > 0)
        {
            Memory::copy(str.asPointer(), data, length * sizeof(CharType));
        }
        data[length] = Utf8Strings::NullCharacter;

        ARK_ASSERT(Utf8::isValid(data, length));
    }

    bool String::contains(StringSlice const& slice) const
    {
        return Utf8Strings::contains(data, length, slice.asPointer(), slice.getLength());
    }

    bool String::containsIgnoreCase(StringSlice const& slice) const
    {
        return Utf8Strings::containsIgnoreCase(data, length, slice.asPointer(), slice.getLength());
    }

    String::SizeType String::findFirst(StringSlice const& pattern, SizeType start) const
    {
        if (pattern.getLength() == 0)
        {
            return Ark::min(start, length);
        }

        if (pattern.getLength() > length || start >= length)
        {
            return InvalidIndex;
        }

        char const* result = Utf8Strings::findFirst(data + start, length - start, pattern.asPointer(), pattern.getLength());
        if (result == nullptr)
        {
            return InvalidIndex;
        }

        return static_cast<SizeType>(result - data);
    }

    String::SizeType String::findLast(StringSlice const& pattern, SizeType end) const
    {
        if (pattern.getLength() == 0)
        {
            return length;
        }

        if (pattern.getLength() > length || end >= length)
        {
            return InvalidIndex;
        }

        // Search in the range [end, length)
        char const* result = Utf8Strings::findLast(data + end, length - end, pattern.asPointer(), pattern.getLength());
        if (result == nullptr)
        {
            return InvalidIndex;
        }

        return static_cast<SizeType>(result - data);
    }

    String::SizeType findFirst(String const& haystack, StringSlice const& needle)
    {
        return haystack.findFirst(needle);
    }

    String::SizeType findLast(String const& haystack, StringSlice const& needle)
    {
        return haystack.findLast(needle);
    }

    bool String::startsWith(StringSlice const& prefix) const
    {
        return Utf8Strings::startsWith(data, length, prefix.asPointer(), prefix.getLength());
    }

    /// Check if the string ends with a given suffix.
    /// @param suffix The suffix to check for.
    /// @return `true` if the string ends with the suffix, otherwise `false`.
    bool String::endsWith(StringSlice const& suffix) const
    {
        return Utf8Strings::endsWith(data, length, suffix.asPointer(), suffix.getLength());
    }

    StringSlice String::substring(SizeType start, SizeType count) const
    {
        ARK_ASSERT(start <= length);

        SizeType actualCount;
        if (count == InvalidIndex)
        {
            actualCount = length - start;
        }
        else
        {
            actualCount = Ark::min(count, length - start);
        }

        return StringSlice(data + start, actualCount);
    }

    StringSlice String::prefix(SizeType maxLength) const
    {
        return substring(0, Ark::min(maxLength, length));
    }

    StringSlice String::suffix(SizeType maxLength) const
    {
        return substring(length - Ark::min(maxLength, length));
    }

    StringSlice String::dropFirst(SizeType k) const
    {
        return substring(Ark::min(k, length));
    }

    StringSlice String::dropLast(SizeType k) const
    {
        return substring(0, length - Ark::min(k, length));
    }

    Collections::Array<StringSlice> String::split(CharType delimiter) const
    {
        ARK_RETURN_VALUE(!isEmpty(), {});

        Collections::Array<StringSlice> result;
        SizeType start = 0;
        SizeType end = 0;

        while ((end = findFirst(delimiter, start)) != InvalidIndex)
        {
            result.append(substring(start, end - start));
            start = end + 1;
        }

        result.append(substring(start));
        return result;
    }

    String String::join(const Collections::Slice<String*> strings, String const& separator)
    {
        if (strings.isEmpty())
        {
            return {};
        }

        SizeType totalLength = 0;
        for (SizeType i = 0; i < strings.getCount(); ++i)
        {
            totalLength += strings[i]->getLength();
        }
        totalLength += (strings.getCount() - 1) * separator.getLength();

        String result;
        result.reserve(totalLength + 1);

        for (SizeType i = 0; i < strings.getCount(); ++i)
        {
            if (i > 0)
            {
                result.append(separator.toSlice());
            }

            result.append(strings[i]->toSlice());
        }

        return result;
    }

    String String::join(const Collections::Slice<String*> strings)
    {
        // Join strings using this string as the separator (empty separator is allowed).
        SizeType totalLength = 0;
        for (SizeType i = 0; i < strings.getCount(); ++i)
        {
            totalLength += strings[i]->getLength();
        }
        totalLength += (strings.getCount() > 0 ? (strings.getCount() - 1) * getLength() : 0);

        String result;
        result.reserve(totalLength + 1);

        for (SizeType i = 0; i < strings.getCount(); ++i)
        {
            if (i > 0)
            {
                result.append(this->toSlice());
            }
            result.append(strings[i]->toSlice());
        }

        return result;
    }

    StringSlice String::trimmed() const
    {
        ARK_RETURN_VALUE(!isEmpty(), {});

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

    StringSlice String::trimmedStart() const
    {
        ARK_RETURN_VALUE(!isEmpty(), {});

        SizeType start = 0;
        while (start < length && Character::isWhitespace(data[start]))
        {
            ++start;
        }

        return substring(start);
    }

    StringSlice String::trimmedEnd() const
    {
        ARK_RETURN_VALUE(!isEmpty(), {});

        SizeType end = length;
        while (end > 0 && Character::isWhitespace(data[end - 1]))
        {
            --end;
        }

        return substring(0, end);
    }

    StringSlice String::strippedPrefix(String const& prefix)
    {
        if (startsWith(prefix.toSlice()))
        {
            return substring(prefix.getLength());
        }

        return *this;
    }

    StringSlice String::strippedSuffix(String const& suffix)
    {
        if (endsWith(suffix.toSlice()))
        {
            return substring(0, length - suffix.getLength());
        }

        return *this;
    }

    String String::toLowercase() const
    {
        String result(*this);

        for (SizeType i = 0; i < result.length; ++i)
        {
            result.data[i] = static_cast<CharType>(Character::toLowercase(result.data[i]));
        }

        return result;
    }

    /// Convert the string to uppercase.
    /// @return A new String with all characters converted to uppercase.
    String String::toUppercase() const
    {
        String result(*this);

        for (SizeType i = 0; i < result.length; ++i)
        {
            result.data[i] = static_cast<CharType>(Character::toUppercase(result.data[i]));
        }

        return result;
    }

    bool String::isValidUtf8() const
    {
        return Utf8::isValid(toSlice());
    }

    String::SizeType String::getCharCount() const
    {
        SizeType count = 0;
        Utf8::forEachCodepoint(toSlice(), [&count](UnicodeChar, usize)
                               {
                                   ++count;
                                   return true;
                               });
        return count;
    }

    bool String::isCharBoundary(SizeType index) const
    {
        if (index == 0 || index == length)
        {
            return true;
        }
        auto prev = Utf8::prevCharBoundary(toSlice(), index);
        return prev && *prev == index;
    }

    Result<StringSlice, String> String::substringChars(SizeType startChar, SizeType charCount) const
    {
        SizeType byteStart = 0;
        SizeType found = 0;
        Utf8::forEachCodepoint(toSlice(), [&found, &byteStart, startChar](UnicodeChar, usize index)
                               {
                                   if (found == startChar)
                                   {
                                       byteStart = static_cast<SizeType>(index);
                                       return false;
                                   }
                                   ++found;
                                   return true;
                               });

        if (found < startChar)
        {
            return Result<StringSlice, String>(unexpectedResult, String("substringChars: startChar out of range"));
        }

        if (charCount == InvalidIndex)
        {
            return StringSlice(data + byteStart, length - byteStart);
        }

        SizeType byteEnd = length;
        SizeType toFind = startChar + charCount;
        found = 0;
        Utf8::forEachCodepoint(toSlice(), [&found, &byteEnd, toFind](UnicodeChar, usize index)
                               {
                                   if (found == toFind)
                                   {
                                       byteEnd = static_cast<SizeType>(index);
                                       return false;
                                   }
                                   ++found;
                                   return true;
                               });

        if (found < toFind)
        {
            byteEnd = length;
        }

        return StringSlice(data + byteStart, byteEnd - byteStart);
    }

    Option<String::SizeType> String::findFirstCodepoint(UnicodeChar codepoint) const
    {
        return Utf8::findFirstCodepoint(toSlice(), codepoint);
    }

    void String::appendCodepoint(UnicodeChar codepoint)
    {
        uint32 cp = static_cast<uint32>(codepoint);
        if (cp > 0x10FFFFu || (cp >= 0xD800u && cp <= 0xDFFFu))
        {
            cp = 0xFFFDu; // U+FFFD replacement character
        }

        char buf[4];
        usize count = 0;
        if (cp <= 0x7Fu)
        {
            buf[0] = static_cast<char>(cp);
            count = 1;
        }
        else if (cp <= 0x7FFu)
        {
            buf[0] = static_cast<char>(0xC0u | ((cp >> 6) & 0x1Fu));
            buf[1] = static_cast<char>(0x80u | (cp & 0x3Fu));
            count = 2;
        }
        else if (cp <= 0xFFFFu)
        {
            buf[0] = static_cast<char>(0xE0u | ((cp >> 12) & 0x0Fu));
            buf[1] = static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
            buf[2] = static_cast<char>(0x80u | (cp & 0x3Fu));
            count = 3;
        }
        else
        {
            buf[0] = static_cast<char>(0xF0u | ((cp >> 18) & 0x07u));
            buf[1] = static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu));
            buf[2] = static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
            buf[3] = static_cast<char>(0x80u | (cp & 0x3Fu));
            count = 4;
        }
        append(buf, static_cast<SizeType>(count));
    }

#pragma region Operators

    String String::operator+(String const& other) const
    {
        String result;

        result.reserve(length + other.getLength());
        result.append(this->toSlice());
        result.append(other.toSlice());

        return result;
    }

    String String::operator+(CharType const* other) const
    {
        String result;
        SizeType otherLength = Utf8Strings::getByteLengthUnsafe(other);

        result.reserve(length + otherLength);
        result.append(this->toSlice());
        result.append(other, otherLength);

        return result;
    }

    String String::operator+(CharType ch) const
    {
        String result;

        result.reserve(length + 1);
        result.append(this->toSlice());
        result.append(ch);

        return result;
    }

    String operator+(String::CharType const* lhs, String const& rhs)
    {
        String result;
        String::SizeType lhsLength = Utf8Strings::getByteLengthUnsafe(lhs);

        result.reserve(lhsLength + rhs.getLength());
        result.append(lhs, lhsLength);
        result.append(rhs.toSlice());

        return result;
    }

    String operator+(String::CharType lhs, String const& rhs)
    {
        String result;

        result.reserve(1 + rhs.getLength());
        result.append(lhs);
        result.append(rhs.toSlice());

        return result;
    }

#pragma endregion

    void String::replace(SizeType start, SizeType end, StringSlice const& slice)
    {
        ARK_ASSERT(start <= end && end <= length);

        CharType const* insertPtr = slice.asPointer();
        SizeType insertLength = slice.getLength();

        // If the replacement aliases this buffer, copy it out first.
        if (data != nullptr && insertPtr != nullptr && insertLength > 0 &&
            insertPtr >= data && insertPtr < data + capacity)
        {
            String owned(insertPtr, insertLength, allocator);
            replace(start, end, owned.toSlice());
            return;
        }

        SizeType removeLength = end - start;
        SizeType newLength = length - removeLength + insertLength;

        if (newLength >= capacity)
        {
            grow(Ark::max(newLength + 1, capacity * 2));
        }

        Memory::move(
            data + end,
            data + start + insertLength,
            static_cast<uint>((length - end) * sizeof(CharType)));

        if (insertLength > 0)
        {
            Memory::copy(
                insertPtr,
                data + start,
                static_cast<uint>(insertLength * sizeof(CharType)));
        }

        length = newLength;
        data[length] = '\0';

        ARK_ASSERT(Utf8::isValid(data, length));
    }

    void String::append(CharType const* characters, SizeType count)
    {
        if (count == 0)
        {
            return;
        }

        const SizeType newLength = length + count;

        if (newLength + 1 >= capacity)
        {
            grow(Ark::max(newLength + 1, capacity * 2));
        }

        memcpy(data + length, characters, count);

        length = newLength;
        data[length] = '\0';

        ARK_ASSERT(Utf8::isValid(data, length));
    }

    void String::append(StringSlice const& slice)
    {
        append(slice.asPointer(), slice.getLength());
    }

    void String::push(CharType const* characters, SizeType count)
    {
        append(characters, count);
    }

    void String::insert(SizeType index, String::CharType ch)
    {
        insert(index, StringSlice(&ch, 1));
    }

    void String::insert(SizeType index, StringSlice const& slice)
    {
        ARK_ASSERT(index <= length);

        SizeType newLength = length + slice.getLength();
        if (newLength >= capacity)
        {
            reserve(newLength + 1);
        }

        memmove(data + index + slice.getLength(),
                data + index,
                length - index);
        memcpy(data + index, slice.asPointer(), slice.getLength());

        length = newLength;
        data[length] = '\0';

        ARK_ASSERT(Utf8::isValid(data, length));
    }

    void String::remove(SizeType start, SizeType end)
    {
        ARK_ASSERT(start <= end && end <= length);

        SizeType removeLength = end - start;

        memmove(data + start,
                data + end,
                length - end);

        length -= removeLength;
        data[length] = Utf8Strings::NullCharacter;
    }

    String String::drain(SizeType start, SizeType end)
    {
        ARK_ASSERT(start <= end && end <= length);

        String result;
        result.reserve(end - start);

        result.append(data + start, end - start);
        remove(start, end);

        return result;
    }

    void String::removeAt(SizeType index)
    {
        ARK_ASSERT(index < length);

        remove(index, index + 1);
    }

    String::SizeType String::removeAll(CharType ch)
    {
        SizeType readPosition = 0;
        SizeType writePosition = 0;
        SizeType removed = 0;

        while (readPosition < length)
        {
            if (data[readPosition] == ch)
            {
                ++readPosition;
                ++removed;
            }
            else
            {
                data[writePosition] = data[readPosition];
                ++writePosition;
                ++readPosition;
            }
        }

        length = writePosition;
        data[length] = Utf8Strings::NullCharacter;

        return removed;
    }

    String::SizeType String::removeAll(StringSlice const& slice)
    {
        if (slice.isEmpty())
        {
            return 0;
        }

        SizeType readPosition = 0;
        SizeType writePosition = 0;
        SizeType removed = 0;

        const SizeType sliceLength = slice.getLength();

        if (sliceLength > length)
        {
            return 0;
        }

        while (readPosition <= length - sliceLength)
        {
            if (memcmp(data + readPosition, slice.asPointer(), sliceLength) == 0)
            {
                readPosition += sliceLength;
                ++removed;
            }
            else
            {
                data[writePosition] = data[readPosition];
                ++writePosition;
                ++readPosition;
            }
        }

        // Copy any remaining characters
        if (writePosition != readPosition)
        {
            memmove(data + writePosition, data + readPosition, length - readPosition);
        }

        length = writePosition + (length - readPosition);
        data[length] = Utf8Strings::NullCharacter;

        return removed;
    }

    bool String::stripPrefix(String const& prefix)
    {
        if (startsWith(prefix.toSlice()))
        {
            drain(0, prefix.getLength());
            return true;
        }

        return false;
    }

    bool String::stripSuffix(String const& suffix)
    {
        if (endsWith(suffix.toSlice()))
        {
            truncate(length - suffix.getLength());
            return true;
        }

        return false;
    }

    String::SizeType String::replace(StringSlice const& from, StringSlice const& to)
    {
        if (from.isEmpty() || from.getLength() > length)
        {
            return 0;
        }

        // If either argument aliases this buffer, materialize copies first.
        bool const fromOverlaps = data != nullptr && from.asPointer() >= data && from.asPointer() < data + capacity;
        bool const toOverlaps = data != nullptr && to.asPointer() >= data && to.asPointer() < data + capacity;
        if (fromOverlaps || toOverlaps)
        {
            String ownedFrom(from, allocator);
            String ownedTo(to, allocator);
            return replace(ownedFrom.toSlice(), ownedTo.toSlice());
        }

        const SizeType fromLength = from.getLength();
        const SizeType toLength = to.getLength();
        SizeType replacements = 0;

        for (SizeType readPosition = 0; readPosition <= length - fromLength;)
        {
            if (memcmp(data + readPosition, from.asPointer(), fromLength) == 0)
            {
                readPosition += fromLength;
                ++replacements;
            }
            else
            {
                ++readPosition;
            }
        }

        if (replacements == 0)
        {
            return 0;
        }

        const SizeType oldLength = length;
        const SizeType newLength = (toLength >= fromLength)
                                     ? oldLength + replacements * (toLength - fromLength)
                                     : oldLength - replacements * (fromLength - toLength);

        if (newLength + 1 > capacity)
        {
            grow(newLength + 1);
        }

        if (toLength > fromLength)
        {
            SizeType readPosition = oldLength;
            SizeType writePosition = newLength;

            while (readPosition > 0)
            {
                if (readPosition >= fromLength && memcmp(data + readPosition - fromLength, from.asPointer(), fromLength) == 0)
                {
                    writePosition -= toLength;
                    memcpy(data + writePosition, to.asPointer(), toLength);
                    readPosition -= fromLength;
                }
                else
                {
                    --readPosition;
                    --writePosition;
                    data[writePosition] = data[readPosition];
                }
            }
        }
        else
        {
            SizeType readPosition = 0;
            SizeType writePosition = 0;

            while (readPosition < oldLength)
            {
                if (readPosition + fromLength <= oldLength && memcmp(data + readPosition, from.asPointer(), fromLength) == 0)
                {
                    if (toLength > 0)
                    {
                        memcpy(data + writePosition, to.asPointer(), toLength);
                        writePosition += toLength;
                    }
                    readPosition += fromLength;
                }
                else
                {
                    data[writePosition] = data[readPosition];
                    ++writePosition;
                    ++readPosition;
                }
            }
        }

        length = newLength;
        data[length] = Utf8Strings::NullCharacter;

        ARK_ASSERT(Utf8::isValid(data, length));

        return replacements;
    }

#pragma region Operators

    String& String::operator+=(String const& other)
    {
        append(other.toSlice());
        return *this;
    }

    String& String::operator+=(CharType const* other)
    {
        SizeType otherLength = Utf8Strings::getByteLengthUnsafe(other);
        append(other, otherLength);
        return *this;
    }

    String& String::operator+=(CharType ch)
    {
        append(ch);
        return *this;
    }

#pragma endregion

    StringSlice const String::toSlice() const
    {
        return StringSlice(data, length);
    }

    StringSlice String::toSlice()
    {
        return StringSlice(data, length);
    }

    String StringSlice::toString() const
    {
        return String(data, length);
    }

    String& String::assign(String const& other)
    {
        if (this == &other)
        {
            return *this;
        }

        if (capacity < other.length + 1)
        {
            allocator.deallocate(const_cast<CharType*>(data));
            capacity = other.length + 1;
            data = allocator.allocateArray<CharType>(capacity);
        }

        length = other.length;
        if (length > 0)
        {
            Memory::copy(other.data, data, length * sizeof(CharType));
        }

        data[length] = Utf8Strings::NullCharacter;

        ARK_ASSERT(Utf8::isValid(data, length));
        return *this;
    }

    String& String::assign(String&& other)
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

            data = other.data;
            length = other.length;
            capacity = other.capacity;

            other.data = nullptr;
            other.length = 0;
            other.capacity = 0;
        }
        else
        {
            assign(static_cast<String const&>(other));
            other.clear();
        }

        ARK_ASSERT(data == nullptr || Utf8::isValid(data, length));
        return *this;
    }

    String& String::assign(CharType const* str)
    {
        if (str == nullptr)
        {
            clear();
            return *this;
        }

        SizeType newLen = Utf8Strings::getByteLengthUnsafe(str);
        return assign(str, newLen);
    }

    String& String::assign(CharType const* str, SizeType count)
    {
        if (str == nullptr)
        {
            clear();
            return *this;
        }

        bool const overlapsSelf = data != nullptr && str >= data && str < data + capacity;

        if (overlapsSelf)
        {
            if (count + 1 > capacity)
            {
                CharType* newData = allocator.allocateArray<CharType>(count + 1);
                Memory::copy(str, newData, count * sizeof(CharType));
                newData[count] = Utf8Strings::NullCharacter;
                allocator.deallocate(const_cast<CharType*>(data));
                data = newData;
                capacity = count + 1;
                length = count;
            }
            else
            {
                Memory::move(str, data, static_cast<uint>(count * sizeof(CharType)));
                length = count;
                data[length] = Utf8Strings::NullCharacter;
            }

            ARK_ASSERT(Utf8::isValid(data, length));
            return *this;
        }

        if (count + 1 > capacity || data == nullptr)
        {
            if (data != nullptr)
            {
                allocator.deallocate(const_cast<CharType*>(data));
            }
            capacity = count + 1;
            data = allocator.allocateArray<CharType>(capacity);
        }

        if (count > 0)
        {
            Memory::copy(str, data, count * sizeof(CharType));
        }

        length = count;
        data[length] = Utf8Strings::NullCharacter;

        ARK_ASSERT(Utf8::isValid(data, length));
        return *this;
    }

    String& String::assign(SizeType count, CharType ch)
    {
        if (count + 1 > capacity)
        {
            allocator.deallocate(const_cast<CharType*>(data));
            capacity = count + 1;
            data = allocator.allocateArray<CharType>(capacity);
        }

        if (count > 0)
        {
            memset(data, ch, count);
        }

        length = count;
        data[length] = Utf8Strings::NullCharacter;

        ARK_ASSERT(Utf8::isValid(data, length));
        return *this;
    }

    String& String::assign(StringSlice const& slice)
    {
        return assign(slice.asPointer(), slice.getLength());
    }

    String& String::assign(Collections::Slice<CharType> characters)
    {
        return assign(characters.asPointer(), characters.getCount());
    }

    String& String::assign(String const& other, SizeType position, SizeType count)
    {
        ARK_ASSERT(position <= other.length);

        SizeType available = other.length - position;
        SizeType toCopy = (count == InvalidIndex) ? available : Ark::min(count, available);
        return assign(other.data + position, toCopy);
    }
}
