#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections
{
    /// A growable bit set stored as packed 64-bit words.
    struct BitSet final
    {
    private:
        static constexpr usize BitsPerWord = 64;

        Array<uint64> words;
        usize bitCount{0};

    public:
#pragma region Types

        using SizeType = usize;
        using WordType = uint64;

#pragma endregion

#pragma region Constructors and Assignments

        BitSet(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : words(allocator)
        {
        }

        explicit BitSet(SizeType count, bool value = false, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : words(allocator)
        {
            resize(count, value);
        }

        BitSet(BitSet const&) = default;
        BitSet(BitSet&&) = default;
        BitSet& operator=(BitSet const&) = default;
        BitSet& operator=(BitSet&&) = default;

        /// Constructs with capacity for at least `bitCapacity` bits (count remains 0).
        static BitSet withCapacity(SizeType bitCapacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
        {
            BitSet result(allocator);
            result.reserve(bitCapacity);
            return result;
        }

        /// Constructs with exactly `count` bits, all cleared.
        static BitSet withCount(SizeType count, Memory::Allocator& allocator = Memory::getDefaultAllocator())
        {
            return BitSet(count, false, allocator);
        }

#pragma endregion

#pragma region Accessors

        /// Returns the number of bits in the set.
        SizeType getCount() const
        {
            return bitCount;
        }

        /// Returns the number of bits that currently fit without reallocation.
        SizeType getCapacity() const
        {
            return words.getCapacity() * BitsPerWord;
        }

        bool isEmpty() const
        {
            return bitCount == 0;
        }

        /// Returns whether any bit is set.
        bool any() const
        {
            SizeType const fullWords = bitCount / BitsPerWord;
            for (SizeType i = 0; i < fullWords; ++i)
            {
                if (words[i] != 0)
                {
                    return true;
                }
            }

            SizeType const remaining = bitCount % BitsPerWord;
            if (remaining != 0)
            {
                uint64 const mask = (uint64{1} << remaining) - 1;
                return (words[fullWords] & mask) != 0;
            }

            return false;
        }

        /// Returns whether no bits are set.
        bool none() const
        {
            return !any();
        }

        /// Returns whether all bits in range `[0, getCount())` are set.
        bool all() const
        {
            if (bitCount == 0)
            {
                return true;
            }

            SizeType const fullWords = bitCount / BitsPerWord;
            for (SizeType i = 0; i < fullWords; ++i)
            {
                if (words[i] != ~uint64{0})
                {
                    return false;
                }
            }

            SizeType const remaining = bitCount % BitsPerWord;
            if (remaining != 0)
            {
                uint64 const mask = (uint64{1} << remaining) - 1;
                return (words[fullWords] & mask) == mask;
            }

            return true;
        }

        /// Returns the number of bits that are set.
        SizeType getSetCount() const
        {
            SizeType total = 0;
            SizeType const fullWords = bitCount / BitsPerWord;
            for (SizeType i = 0; i < fullWords; ++i)
            {
                total += popCount(words[i]);
            }

            SizeType const remaining = bitCount % BitsPerWord;
            if (remaining != 0)
            {
                uint64 const mask = (uint64{1} << remaining) - 1;
                total += popCount(words[fullWords] & mask);
            }

            return total;
        }

        /// Returns whether the bit at `index` is set.
        bool has(SizeType index) const
        {
            ARK_ASSERT(index < bitCount);
            return (words[wordIndex(index)] & bitMask(index)) != 0;
        }

        /// Returns whether the bit at `index` is set, or false if out of range.
        bool tryHas(SizeType index) const
        {
            if (index >= bitCount)
            {
                return false;
            }
            return (words[wordIndex(index)] & bitMask(index)) != 0;
        }

#pragma endregion

#pragma region Modifiers

        void reserve(SizeType bitCapacity)
        {
            words.reserve(wordCountForBits(bitCapacity));
        }

        /// Resizes to `newCount` bits. New bits are initialized to `value`.
        void resize(SizeType newCount, bool value = false)
        {
            SizeType const oldCount = bitCount;
            SizeType const requiredWords = wordCountForBits(newCount);
            words.resize(requiredWords);
            bitCount = newCount;

            if (newCount > oldCount)
            {
                // Ensure unused high bits in the former trailing word start cleared.
                if (oldCount > 0 && (oldCount % BitsPerWord) != 0)
                {
                    SizeType const oldWord = wordIndex(oldCount - 1);
                    uint64 const keepMask = (uint64{1} << (oldCount % BitsPerWord)) - 1;
                    words[oldWord] &= keepMask;
                }

                if (value)
                {
                    for (SizeType i = oldCount; i < newCount; ++i)
                    {
                        words[wordIndex(i)] |= bitMask(i);
                    }
                }
            }

            maskTrailingBits();
        }

        /// Appends one bit at the end.
        void append(bool value)
        {
            SizeType const index = bitCount;
            resize(bitCount + 1, false);
            if (value)
            {
                set(index);
            }
        }

        void set(SizeType index)
        {
            ARK_ASSERT(index < bitCount);
            words[wordIndex(index)] |= bitMask(index);
        }

        void set(SizeType index, bool value)
        {
            if (value)
            {
                set(index);
            }
            else
            {
                clear(index);
            }
        }

        void clear(SizeType index)
        {
            ARK_ASSERT(index < bitCount);
            words[wordIndex(index)] &= ~bitMask(index);
        }

        void toggle(SizeType index)
        {
            ARK_ASSERT(index < bitCount);
            words[wordIndex(index)] ^= bitMask(index);
        }

        void setAll()
        {
            if (bitCount == 0)
            {
                return;
            }

            SizeType const fullWords = bitCount / BitsPerWord;
            for (SizeType i = 0; i < fullWords; ++i)
            {
                words[i] = ~uint64{0};
            }

            SizeType const remaining = bitCount % BitsPerWord;
            if (remaining != 0)
            {
                words[fullWords] = (uint64{1} << remaining) - 1;
            }
        }

        void clearAll()
        {
            for (SizeType i = 0; i < words.getCount(); ++i)
            {
                words[i] = 0;
            }
        }

        void toggleAll()
        {
            if (bitCount == 0)
            {
                return;
            }

            SizeType const fullWords = bitCount / BitsPerWord;
            for (SizeType i = 0; i < fullWords; ++i)
            {
                words[i] = ~words[i];
            }

            SizeType const remaining = bitCount % BitsPerWord;
            if (remaining != 0)
            {
                uint64 const mask = (uint64{1} << remaining) - 1;
                words[fullWords] = (~words[fullWords]) & mask;
            }
        }

        void removeAll()
        {
            words.removeAll();
            bitCount = 0;
        }

        BitSet& operator&=(BitSet const& other)
        {
            SizeType const shared = Ark::min(bitCount, other.bitCount);
            SizeType const sharedWords = wordCountForBits(shared);
            for (SizeType i = 0; i < sharedWords; ++i)
            {
                words[i] &= (i < other.words.getCount()) ? other.words[i] : uint64{0};
            }

            // Bits beyond the other set become 0.
            if (bitCount > other.bitCount)
            {
                for (SizeType i = other.bitCount; i < bitCount; ++i)
                {
                    clear(i);
                }
            }

            maskTrailingBits();
            return *this;
        }

        BitSet& operator|=(BitSet const& other)
        {
            if (other.bitCount > bitCount)
            {
                resize(other.bitCount, false);
            }

            SizeType const wordLimit = Ark::min(words.getCount(), other.words.getCount());
            for (SizeType i = 0; i < wordLimit; ++i)
            {
                words[i] |= other.words[i];
            }

            maskTrailingBits();
            return *this;
        }

        BitSet& operator^=(BitSet const& other)
        {
            if (other.bitCount > bitCount)
            {
                resize(other.bitCount, false);
            }

            SizeType const wordLimit = Ark::min(words.getCount(), other.words.getCount());
            for (SizeType i = 0; i < wordLimit; ++i)
            {
                words[i] ^= other.words[i];
            }

            maskTrailingBits();
            return *this;
        }

        void swap(BitSet& other)
        {
            Array<uint64> temporaryWords = Ark::move(words);
            SizeType temporaryCount = bitCount;

            words = Ark::move(other.words);
            bitCount = other.bitCount;

            other.words = Ark::move(temporaryWords);
            other.bitCount = temporaryCount;
        }

#pragma endregion

#pragma region Operators

        bool operator[](SizeType index) const
        {
            return has(index);
        }

        bool operator==(BitSet const& other) const
        {
            if (bitCount != other.bitCount)
            {
                return false;
            }

            SizeType const fullWords = bitCount / BitsPerWord;
            for (SizeType i = 0; i < fullWords; ++i)
            {
                if (words[i] != other.words[i])
                {
                    return false;
                }
            }

            SizeType const remaining = bitCount % BitsPerWord;
            if (remaining != 0)
            {
                uint64 const mask = (uint64{1} << remaining) - 1;
                if ((words[fullWords] & mask) != (other.words[fullWords] & mask))
                {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(BitSet const& other) const
        {
            return !(*this == other);
        }

#pragma endregion

#pragma region STL Compatibility API

        using size_type = SizeType;

        size_type size() const
        {
            return getCount();
        }

        bool empty() const
        {
            return isEmpty();
        }

        bool test(SizeType index) const
        {
            return has(index);
        }

        size_type count() const
        {
            return getSetCount();
        }

        void reset()
        {
            clearAll();
        }

        void reset(SizeType index)
        {
            clear(index);
        }

        void flip(SizeType index)
        {
            toggle(index);
        }

        void flip()
        {
            toggleAll();
        }

        void clear()
        {
            removeAll();
        }

#pragma endregion

    private:
        static SizeType wordIndex(SizeType bitIndex)
        {
            return bitIndex / BitsPerWord;
        }

        static uint64 bitMask(SizeType bitIndex)
        {
            return uint64{1} << (bitIndex % BitsPerWord);
        }

        static SizeType wordCountForBits(SizeType bits)
        {
            return bits == 0 ? 0 : (bits + BitsPerWord - 1) / BitsPerWord;
        }

        static SizeType popCount(uint64 value)
        {
#if defined(__GNUC__) || defined(__clang__)
            return static_cast<SizeType>(__builtin_popcountll(value));
#else
            SizeType count = 0;
            while (value != 0)
            {
                value &= value - 1;
                ++count;
            }
            return count;
#endif
        }

        void maskTrailingBits()
        {
            if (bitCount == 0 || words.isEmpty())
            {
                return;
            }

            SizeType const remaining = bitCount % BitsPerWord;
            if (remaining != 0)
            {
                uint64 const mask = (uint64{1} << remaining) - 1;
                words[words.getCount() - 1] &= mask;
            }
        }
    };

    inline BitSet operator&(BitSet left, BitSet const& right)
    {
        left &= right;
        return left;
    }

    inline BitSet operator|(BitSet left, BitSet const& right)
    {
        left |= right;
        return left;
    }

    inline BitSet operator^(BitSet left, BitSet const& right)
    {
        left ^= right;
        return left;
    }
}
