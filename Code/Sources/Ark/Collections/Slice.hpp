#pragma once

#include "Ark/Collections/Algorithms.hpp"
#include "Ark/Collections/Internal/RandomAccessIterator.hpp"
#include "Ark/Collections/Internal/ReverseIterator.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Traits.hpp"

namespace Ark::Collections
{
    template <typename T>
    struct Slice final
    {
#pragma region Types

    public:
        /// The type of the collection.
        using CollectionType = Slice;

        /// The type of values viewed by the slice.
        using ValueType = T;

        /// The type of references to values in the slice.
        using Reference = ValueType&;

        /// The type of const references to values in the slice.
        using ConstReference = const ValueType&;

        /// The type of pointers to values in the slice.
        using Pointer = ValueType*;

        /// The type of const pointers to values in the slice.
        using ConstPointer = const ValueType*;

        /// The type used for sizes and indices.
        using SizeType = usize;

        /// The type used for pointer differences.
        using DifferenceType = isize;

#pragma endregion

    protected:
        Pointer data;
        SizeType count;

#pragma region Constructors

    public:
        constexpr Slice()
            : data{nullptr}
            , count{0}
        {
        }

        constexpr Slice(Pointer ptr, SizeType length)
            : data{ptr}
            , count{length}
        {
        }

        template <usize N>
        constexpr Slice(ValueType (&array)[N])
            : data{array}
            , count{N}
        {
        }

        template <typename U>
        constexpr Slice(InitializerList<U> init) = delete;

#pragma endregion

#pragma region Iterators

    public:
        /// The iterator for the slice.
        using Iterator = Internal::RandomAccessIterator<Slice>;

        /// The const iterator for the slice.
        using ConstIterator = Internal::ConstRandomAccessIterator<Slice>;

        /// The reverse iterator for the slice.
        using ReverseIterator = Internal::ReverseIterator<Iterator>;

        /// The const reverse iterator for the slice.
        using ConstReverseIterator = Internal::ReverseIterator<ConstIterator>;

        /// Gets an iterator to the beginning of the slice.
        /// @return An iterator to the beginning of the slice.
        constexpr Iterator getStartIterator()
        {
            return Iterator(data);
        }

        /// Gets a const iterator to the beginning of the slice.
        /// @return A const iterator to the beginning of the slice.
        constexpr ConstIterator getStartIterator() const
        {
            return ConstIterator(data);
        }

        /// Gets an iterator to the end of the slice.
        /// @return An iterator to the end of the slice.
        constexpr Iterator getEndIterator()
        {
            return Iterator(data + count);
        }

        /// Gets a const iterator to the end of the slice.
        /// @return A const iterator to the end of the slice.
        constexpr ConstIterator getEndIterator() const
        {
            return ConstIterator(data + count);
        }

        /// Gets a reverse iterator to the end of the slice.
        /// @return A reverse iterator to the end of the slice.
        constexpr ReverseIterator getStartReverseIterator()
        {
            return ReverseIterator(getEndIterator());
        }

        /// Gets a const reverse iterator to the end of the slice.
        /// @return A const reverse iterator to the end of the slice.
        constexpr ConstReverseIterator getStartReverseIterator() const
        {
            return ConstReverseIterator(getEndIterator());
        }

        /// Gets a reverse iterator to the beginning of the slice.
        /// @return A reverse iterator to the beginning of the slice.
        constexpr ReverseIterator getEndReverseIterator()
        {
            return ReverseIterator(getStartIterator());
        }

        /// Gets a const reverse iterator to the beginning of the slice.
        /// @return A const reverse iterator to the beginning of the slice.
        constexpr ConstReverseIterator getEndReverseIterator() const
        {
            return ConstReverseIterator(getStartIterator());
        }

#pragma endregion

#pragma region Accessors

    public:
        /// Checks if the slice is empty.
        /// @return `true` if the slice is empty, otherwise `false`.
        constexpr bool isEmpty() const
        {
            return count == 0;
        }

        /// Gets the number of elements in the slice.
        /// @return The number of elements in the slice.
        constexpr SizeType getCount() const
        {
            return count;
        }

        /// Gets the number of bytes in the array.
        /// @return The number of bytes in the array.
        constexpr SizeType getByteCount() const
        {
            return count * sizeof(T);
        }

        /// Gets a pointer to the underlying data.
        /// @return A pointer to the underlying data.
        constexpr Pointer asPointer()
        {
            return data;
        }

        /// Gets a pointer to the underlying data.
        /// @return A pointer to the underlying data.
        constexpr ConstPointer asPointer() const
        {
            return data;
        }

        /// Gets a const reference to the first element.
        /// @return A const reference to the first element.
        constexpr ConstReference getFirst() const
        {
            ARK_ASSERT(!isEmpty());

            return data[0];
        }

        /// Gets a const reference to the last element.
        /// @return A const reference to the last element.
        constexpr ConstReference getLast() const
        {
            ARK_ASSERT(!isEmpty());

            return data[count - 1];
        }

        /// Returns a reference to the element at the given index, without bounds checking.
        /// @param index The index of the element to access.
        /// @return A reference to the element at the given index.
        constexpr Reference get(SizeType index)
        {
            ARK_ASSERT(index < count);

            return data[index];
        }

        /// Returns a const reference to the element at the given index, without bounds checking.
        /// @param index The index of the element to access.
        /// @return A const reference to the element at the given index.
        constexpr ConstReference get(SizeType index) const
        {
            ARK_ASSERT(index < count);

            return data[index];
        }

        /// Returns an Option containing a reference to the element at the given index.
        /// @param index The index of the element to access.
        /// @return An Option containing a reference to the element if the index is valid, None otherwise.
        constexpr Option<Reference> tryGet(SizeType index)
        {
            if (index < count)
            {
                return data[index];
            }

            return none;
        }

        /// Returns an Option containing a const reference to the element at the given index.
        /// @param index The index of the element to access.
        /// @return An Option containing a const reference to the element if the index is valid, None otherwise.
        constexpr Option<ConstReference> tryGet(SizeType index) const
        {
            if (index < count)
            {
                return data[index];
            }

            return none;
        }

#pragma endregion

#pragma region Operators

        constexpr Reference operator[](SizeType index)
        {
            ARK_ASSERT(index < count);

            return data[index];
        }

        constexpr ConstReference operator[](SizeType index) const
        {
            ARK_ASSERT(index < count);

            return data[index];
        }

        constexpr operator Slice<const T>() const
        {
            return Slice<const T>(data, count);
        }

        constexpr Slice& operator=(InitializerList<Traits::RemoveCVType<T>> init) = delete;

#pragma endregion

#pragma region Utilities

        /// Creates a sub-slice.
        /// @param start The start index of the sub-slice.
        /// @param end The end index of the sub-slice.
        /// @return A new Slice representing the sub-slice.
        constexpr Slice subslice(SizeType start, SizeType end) const
        {
            ARK_ASSERT(start <= end && end <= count);

            return Slice(data + start, end - start);
        }

        /// Creates a sub-slice from a given start index to the end.
        /// @param start The start index of the sub-slice.
        /// @return A new Slice representing the sub-slice.
        constexpr Slice subslice(SizeType start) const
        {
            ARK_ASSERT(start <= count);

            return Slice(data + start, count - start);
        }

        /// Checks if the slice contains the specified value.
        /// @param value The value to search for.
        /// @return `true` if the value is found, otherwise `false`.
        constexpr bool contains(T const& value) const
        {
            return Collections::find(getStartIterator(), getEndIterator(), value) != getEndIterator();
        }

        /*template <typename F>
        /// Transforms each element and returns the mapped result.
        auto map(F&& func) const
        {
            using ResultType = decltype(func(std::declval<T>()));
            Array<ResultType> result;
            result.reserve(count);
            for (usize i = 0; i < count; ++i)
            {
                result.append(func(data[i]));
            }
            return result;
        }*/

#pragma endregion
    };

#pragma region Standard Library Interface

    /// Returns an iterator to the first element.
    template <typename T>
    constexpr auto begin(Slice<T>& array)
    {
        return array.getStartIterator();
    }

    /// Returns an iterator to one-past-the-last element.
    template <typename T>
    constexpr auto end(Slice<T>& array)
    {
        return array.getEndIterator();
    }

    /// Returns an iterator to the first element.
    template <typename T>
    constexpr auto begin(Slice<T> const& array)
    {
        return array.getStartIterator();
    }

    /// Returns an iterator to one-past-the-last element.
    template <typename T>
    constexpr auto end(Slice<T> const& array)
    {
        return array.getEndIterator();
    }

    /// Returns a pointer to the underlying data.
    template <typename T>
    constexpr auto data(Slice<T>& array)
    {
        return array.asPointer();
    }

    /// Returns a pointer to the underlying data.
    template <typename T>
    constexpr auto data(Slice<T> const& array)
    {
        return array.asPointer();
    }

    /// Returns the element count.
    template <typename T>
    constexpr auto size(Slice<T> const& array)
    {
        return array.getCount();
    }

    /// Returns whether the range is empty.
    template <typename T>
    constexpr bool empty(Slice<T> const& array)
    {
        return array.isEmpty();
    }

#pragma endregion
}
