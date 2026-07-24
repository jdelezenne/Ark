#pragma once

#include "Ark/Collections/Algorithms.hpp"
#include "Ark/Collections/Internal/RandomAccessIterator.hpp"
#include "Ark/Collections/Internal/ReverseIterator.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Option.hpp"

namespace Ark::Collections
{
    /// Fixed-size non-owning slice over contiguous values.
    /// The slice references external storage and never manages memory ownership.
    template <typename T, usize N>
    struct InlineSlice final
    {
#pragma region Types

    public:
        using CollectionType = InlineSlice;
        using ValueType = T;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using SizeType = usize;
        using DifferenceType = isize;

#pragma endregion

    protected:
        Pointer data;

#pragma region Constructors and Assignments

    public:
        constexpr InlineSlice()
            : data{nullptr}
        {
        }

        constexpr InlineSlice(Pointer ptr)
            : data{ptr}
        {
        }

        constexpr InlineSlice(ValueType (&array)[N])
            : data{array}
        {
        }

#pragma endregion

#pragma region Iterators

    public:
        using Iterator = Internal::RandomAccessIterator<InlineSlice>;
        using ConstIterator = Internal::ConstRandomAccessIterator<InlineSlice>;
        using ReverseIterator = Internal::ReverseIterator<Iterator>;
        using ConstReverseIterator = Internal::ReverseIterator<ConstIterator>;

        /// Returns an iterator to the first element.
        constexpr Iterator getStartIterator()
        {
            return Iterator(data);
        }

        /// Returns an iterator to the first element.
        constexpr ConstIterator getStartIterator() const
        {
            return ConstIterator(data);
        }

        /// Returns an iterator to one-past-the-last element.
        constexpr Iterator getEndIterator()
        {
            return Iterator(data + N);
        }

        /// Returns an iterator to one-past-the-last element.
        constexpr ConstIterator getEndIterator() const
        {
            return ConstIterator(data + N);
        }

        /// Returns start reverse iterator.
        constexpr ReverseIterator getStartReverseIterator()
        {
            return ReverseIterator(getEndIterator());
        }

        /// Returns start reverse iterator.
        constexpr ConstReverseIterator getStartReverseIterator() const
        {
            return ConstReverseIterator(getEndIterator());
        }

        /// Returns end reverse iterator.
        constexpr ReverseIterator getEndReverseIterator()
        {
            return ReverseIterator(getStartIterator());
        }

        /// Returns end reverse iterator.
        constexpr ConstReverseIterator getEndReverseIterator() const
        {
            return ConstReverseIterator(getStartIterator());
        }

#pragma endregion

#pragma region Accessors

    public:
        /// Returns whether it is empty.
        constexpr bool isEmpty() const
        {
            return N == 0;
        }

        /// Returns the number of stored elements.
        constexpr SizeType getCount() const
        {
            return N;
        }

        /// Returns byte count.
        constexpr SizeType getByteCount() const
        {
            return N * sizeof(T);
        }

        /// Returns a pointer to the underlying data.
        constexpr Pointer asPointer()
        {
            return data;
        }

        /// Returns a pointer to the underlying data.
        constexpr ConstPointer asPointer() const
        {
            return data;
        }

        /// Returns first.
        constexpr ConstReference getFirst() const
        {
            ARK_ASSERT(!isEmpty());
            return data[0];
        }

        /// Returns last.
        constexpr ConstReference getLast() const
        {
            ARK_ASSERT(!isEmpty());
            return data[N - 1];
        }

        /// Returns the requested value.
        constexpr Reference get(SizeType index)
        {
            ARK_ASSERT(index < N);
            return data[index];
        }

        /// Returns the requested value.
        constexpr ConstReference get(SizeType index) const
        {
            ARK_ASSERT(index < N);
            return data[index];
        }

        /// Attempts to get.
        constexpr Option<Reference> tryGet(SizeType index)
        {
            if (index < N)
            {
                return data[index];
            }
            return none;
        }

        /// Attempts to get.
        constexpr Option<ConstReference> tryGet(SizeType index) const
        {
            if (index < N)
            {
                return data[index];
            }
            return none;
        }

        /// Checks if the slice contains the specified value.
        /// @param value The value to search for.
        /// @return `true` if the value is found, otherwise `false`.
        constexpr bool contains(T const& value) const
        {
            return Collections::find(getStartIterator(), getEndIterator(), value) != getEndIterator();
        }

#pragma endregion

#pragma region Operators

        constexpr Reference operator[](SizeType index)
        {
            ARK_ASSERT(index < N);
            return data[index];
        }

        constexpr ConstReference operator[](SizeType index) const
        {
            ARK_ASSERT(index < N);
            return data[index];
        }

        constexpr operator InlineSlice<const T, N>() const
        {
            return InlineSlice<const T, N>(data);
        }

#pragma endregion
    };

#pragma region STL Compatibility API

    template <typename T, usize N>
    constexpr auto begin(InlineSlice<T, N>& array)
    {
        return array.getStartIterator();
    }

    template <typename T, usize N>
    constexpr auto end(InlineSlice<T, N>& array)
    {
        return array.getEndIterator();
    }

    template <typename T, usize N>
    constexpr auto begin(InlineSlice<T, N> const& array)
    {
        return array.getStartIterator();
    }

    template <typename T, usize N>
    constexpr auto end(InlineSlice<T, N> const& array)
    {
        return array.getEndIterator();
    }

    template <typename T, usize N>
    constexpr auto data(InlineSlice<T, N>& array)
    {
        return array.asPointer();
    }

    template <typename T, usize N>
    constexpr auto data(InlineSlice<T, N> const& array)
    {
        return array.asPointer();
    }

    template <typename T, usize N>
    constexpr auto size(InlineSlice<T, N> const&)
    {
        return N;
    }

    template <typename T, usize N>
    constexpr bool empty(InlineSlice<T, N> const&)
    {
        return N == 0;
    }

#pragma endregion
}
