#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Collections::Internal
{
    /// A reverse iterator adaptor.
    /// @tparam Iterator The type of the underlying iterator.
    template <typename Iterator>
    struct ReverseIterator final
    {
        using ValueType = Iterator::ValueType;           ///< The type of the value stored in the collection.
        using DifferenceType = Iterator::DifferenceType; ///< The type used for pointer differences.
        using Pointer = Iterator::Pointer;               ///< The type of pointers to values in the collection.
        using Reference = Iterator::Reference;           ///< The type of references to values in the collection.

#pragma region Std Interface
        using value_type = Iterator::value_type;
        using difference_type = Iterator::difference_type;
        using pointer = Iterator::pointer;
        using reference = Iterator::reference;
#pragma endregion

    protected:
        Iterator current;

    public:
        /// Default constructor
        constexpr ReverseIterator()
            : current()
        {
        }

        /// Constructor from iterator
        constexpr explicit ReverseIterator(Iterator iter)
            : current{iter}
        {
        }

        /// Copy constructor
        constexpr ReverseIterator(const ReverseIterator& other)
            : current{other.current}
        {
        }

        /// Constructor from another reverse iterator type
        template <typename OtherIterator>
        constexpr ReverseIterator(const ReverseIterator<OtherIterator>& other)
            : current{other.base()}
        {
        }

        /// Get the underlying iterator
        constexpr Iterator base() const
        {
            return current;
        }

        constexpr Reference operator*() const
        {
            Iterator tmp = current;
            return *--tmp;
        }

        constexpr Pointer operator->() const
        {
            return &(operator*());
        }

        constexpr reference operator[](DifferenceType n) const
        {
            return *(*this + n);
        }

        constexpr ReverseIterator& operator++()
        {
            --current;
            return *this;
        }

        constexpr ReverseIterator operator++(int)
        {
            Internal::ReverseIterator tmp = *this;
            --current;
            return tmp;
        }

        constexpr ReverseIterator& operator--()
        {
            ++current;
            return *this;
        }

        constexpr ReverseIterator operator--(int)
        {
            Internal::ReverseIterator tmp = *this;
            ++current;
            return tmp;
        }

        constexpr ReverseIterator& operator+=(DifferenceType n)
        {
            current -= n;
            return *this;
        }

        constexpr ReverseIterator& operator-=(DifferenceType n)
        {
            current += n;
            return *this;
        }

        constexpr ReverseIterator operator+(DifferenceType n) const
        {
            return ReverseIterator(current - n);
        }

        constexpr ReverseIterator operator-(DifferenceType n) const
        {
            return ReverseIterator(current + n);
        }

        constexpr DifferenceType operator-(const ReverseIterator& other) const
        {
            return other.current - current;
        }
    };

    template <typename Iterator1, typename Iterator2>
    constexpr bool operator==(ReverseIterator<Iterator1> const& lhs,
                              ReverseIterator<Iterator2> const& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <typename Iterator1, typename Iterator2>
    constexpr bool operator!=(ReverseIterator<Iterator1> const& lhs,
                              ReverseIterator<Iterator2> const& rhs)
    {
        return lhs.base() != rhs.base();
    }

    template <typename Iterator1, typename Iterator2>
    constexpr bool operator<(ReverseIterator<Iterator1> const& lhs,
                             ReverseIterator<Iterator2> const& rhs)
    {
        return lhs.base() > rhs.base();
    }

    template <typename Iterator1, typename Iterator2>
    constexpr bool operator>(ReverseIterator<Iterator1> const& lhs,
                             ReverseIterator<Iterator2> const& rhs)
    {
        return lhs.base() < rhs.base();
    }

    template <typename Iterator1, typename Iterator2>
    constexpr bool operator<=(ReverseIterator<Iterator1> const& lhs,
                              ReverseIterator<Iterator2> const& rhs)
    {
        return lhs.base() >= rhs.base();
    }

    template <typename Iterator1, typename Iterator2>
    constexpr bool operator>=(ReverseIterator<Iterator1> const& lhs,
                              ReverseIterator<Iterator2> const& rhs)
    {
        return lhs.base() <= rhs.base();
    }

    template <typename Iterator>
    constexpr ReverseIterator<Iterator> operator+(
        typename ReverseIterator<Iterator>::DifferenceType n,
        const ReverseIterator<Iterator>& iter)
    {
        return ReverseIterator<Iterator>(iter.base() - n);
    }

    /// Make reverse iterator function.
    template <typename Iterator>
    constexpr ReverseIterator<Iterator> makeReverseIterator(Iterator iter)
    {
        return ReverseIterator<Iterator>(iter);
    }
}
