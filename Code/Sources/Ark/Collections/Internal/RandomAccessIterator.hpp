#pragma once

// #include "Ark/Core/Internal/Std.hpp"
#include "Ark/Collections/Internal/Traits.hpp"

namespace Ark::Collections::Internal
{
    /// Base iterator for random access iterators.
    /// @tparam Collection The collection type.
    /// @tparam IsConst Whether the iterator is a const iterator or a mutable iterator.
    template <typename Collection, bool IsConst>
    struct RandomAccessIteratorBase final
    {
    private:
        /// The type of the element.
        using ElementType = Traits::ConditionalType<IsConst,
                                                    const typename Collection::ValueType,
                                                    typename Collection::ValueType>;

        /// The type of the pointer to the element.
        using ElementPointer = ElementType*;

        ElementPointer ptr; ///< The pointer to the current element.

    public:
        using ValueType = ElementType;                              ///< The type of the value stored in the collection.
        using DifferenceType = typename Collection::DifferenceType; ///< The type used for pointer differences.
        using Pointer = ElementType*;                               ///< The type of pointers to values in the collection.
        using Reference = ElementType&;                             ///< The type of references to values in the collection.

#pragma region Std Interface
        using value_type = ValueType;
        using difference_type = DifferenceType;
        using pointer = Pointer;
        using reference = Reference;
#pragma endregion

        constexpr explicit RandomAccessIteratorBase(ElementPointer ptr)
            : ptr{ptr}
        {
        }

        constexpr RandomAccessIteratorBase(const RandomAccessIteratorBase<Collection, false>& iter)
            : ptr{iter.ptr}
        {
        }

        constexpr Reference operator*() const
        {
            return *ptr;
        }

        constexpr Pointer operator->() const
        {
            return ptr;
        }

        constexpr RandomAccessIteratorBase& operator++()
        {
            ++ptr;
            return *this;
        }

        constexpr RandomAccessIteratorBase operator++(int)
        {
            auto tmp = *this;
            ++ptr;
            return tmp;
        }

        constexpr RandomAccessIteratorBase& operator--()
        {
            --ptr;
            return *this;
        }

        constexpr RandomAccessIteratorBase operator--(int)
        {
            auto tmp = *this;
            --ptr;
            return tmp;
        }

        constexpr RandomAccessIteratorBase& operator+=(DifferenceType n)
        {
            ptr += n;
            return *this;
        }

        constexpr RandomAccessIteratorBase operator+(DifferenceType n) const
        {
            return RandomAccessIteratorBase(ptr + n);
        }

        constexpr RandomAccessIteratorBase& operator-=(DifferenceType n)
        {
            ptr -= n;
            return *this;
        }

        constexpr RandomAccessIteratorBase operator-(DifferenceType n) const
        {
            return RandomAccessIteratorBase(ptr - n);
        }

        constexpr DifferenceType operator-(const RandomAccessIteratorBase& other) const
        {
            return ptr - other.ptr;
        }

        constexpr Reference operator[](DifferenceType n) const
        {
            return ptr[n];
        }

        constexpr bool operator==(const RandomAccessIteratorBase& other) const
        {
            return ptr == other.ptr;
        }

        constexpr bool operator!=(const RandomAccessIteratorBase& other) const
        {
            return ptr != other.ptr;
        }

        constexpr bool operator<(const RandomAccessIteratorBase& other) const
        {
            return ptr < other.ptr;
        }

        constexpr bool operator>(const RandomAccessIteratorBase& other) const
        {
            return ptr > other.ptr;
        }

        constexpr bool operator<=(const RandomAccessIteratorBase& other) const
        {
            return ptr <= other.ptr;
        }

        constexpr bool operator>=(const RandomAccessIteratorBase& other) const
        {
            return ptr >= other.ptr;
        }

        template <typename OtherColection, bool OtherIsConst>
        friend struct RandomAccessIteratorBase;
    };

    /// Alias for a random access iterator.
    /// @tparam T The type of the elements.
    template <typename T>
    using RandomAccessIterator = RandomAccessIteratorBase<T, false>;

    /// Alias for a const random access iterator.
    /// @tparam T The type of the elements.
    template <typename T>
    using ConstRandomAccessIterator = RandomAccessIteratorBase<T, true>;
}
