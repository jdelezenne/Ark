#pragma once

#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Collections
{
    /// Calculates the distance between two iterators.
    /// @tparam Iterator The type of the iterator.
    /// @param startIter The starting iterator.
    /// @param endIter The ending iterator (exclusive).
    /// @return The distance between the two iterators.
    /// Returns the distance between iterators.
    template <typename Iterator>
    constexpr Iterator::DifferenceType distance(Iterator startIter, Iterator endIter)
    {
        ARK_ASSERT(startIter <= endIter);

        return endIter - startIter;
    }

    template <typename T = void>
    struct EqualTo final
    {
        constexpr bool operator()(T const& lhs, T const& rhs) const
        {
            return lhs == rhs;
        }
    };

    template <typename T = void>
    struct NotEqualTo final
    {
        constexpr bool operator()(T const& lhs, T const& rhs) const
        {
            return lhs != rhs;
        }
    };

    template <>
    struct EqualTo<void> final
    {
        template <typename T1, typename T2>
        constexpr auto operator()(T1&& lhs, T2&& rhs) const
            /// Deduces and returns the declared type expression.
            -> decltype(static_cast<T1&&>(lhs) == static_cast<T2&&>(rhs))
        {
            return static_cast<T1&&>(lhs) == static_cast<T2&&>(rhs);
        }
    };

    template <>
    struct NotEqualTo<void> final
    {
        template <typename T1, typename T2>
        constexpr auto operator()(T1&& lhs, T2&& rhs) const
            /// Deduces and returns the declared type expression.
            -> decltype(static_cast<T1&&>(lhs) != static_cast<T2&&>(rhs))
        {
            return static_cast<T1&&>(lhs) != static_cast<T2&&>(rhs);
        }
    };

    template <typename T = void>
    struct Less final
    {
        constexpr bool operator()(T const& lhs, T const& rhs) const
        {
            return lhs < rhs;
        }
    };

    template <>
    struct Less<void> final
    {
        template <typename T1, typename T2>
        constexpr auto operator()(T1&& lhs, T2&& rhs) const
            -> decltype(static_cast<T1&&>(lhs) < static_cast<T2&&>(rhs))
        {
            return static_cast<T1&&>(lhs) < static_cast<T2&&>(rhs);
        }
    };

    /// Compares two ranges defined by iterators for equality using a predicate.
    /// @tparam Iterator1 The type of the first iterator.
    /// @tparam Iterator2 The type of the second iterator.
    /// @tparam Predicate The type of the predicate used for comparison.
    /// @param startIter1 The starting iterator of the first range.
    /// @param endIter1 The ending iterator of the first range (exclusive).
    /// @param startIter2 The starting iterator of the second range.
    /// @param predicate The predicate used to compare elements from both ranges.
    /// @return `true` if the elements in both ranges are equal according to the predicate, otherwise `false`.
    template <typename Iterator1, typename Iterator2, typename Predicate>
    constexpr bool equal(
        Iterator1 const startIter1,
        Iterator1 const endIter1,
        Iterator2 const startIter2,
        Predicate predicate)
    {
        Iterator1 iter1 = startIter1;
        Iterator2 iter2 = startIter2;

        for (; iter1 != endIter1; ++iter1, ++iter2)
        {
            if (!predicate(*iter1, *iter2))
            {
                return false;
            }
        }

        return true;
    }

    /// Compares two ranges defined by iterators for equality using a predicate.
    /// @tparam Iterator1 The type of the first iterator.
    /// @tparam Iterator2 The type of the second iterator.
    /// @tparam Predicate The type of the predicate used for comparison.
    /// @param startIter1 The starting iterator of the first range.
    /// @param endIter1 The ending iterator of the first range (exclusive).
    /// @param startIter2 The starting iterator of the second range.
    /// @param endIter2 The ending iterator of the second range (exclusive).
    /// @param predicate The predicate used to compare elements from both ranges.
    /// @return `true` if the elements in both ranges are equal according to the predicate and both ranges have the same length, otherwise `false`.
    template <typename Iterator1, typename Iterator2, typename Predicate>
    constexpr bool equal(
        Iterator1 const startIter1,
        Iterator1 const endIter1,
        Iterator2 const startIter2,
        Iterator2 const endIter2,
        Predicate predicate)
    {
        Iterator1 iter1 = startIter1;
        Iterator2 iter2 = startIter2;

        while (true)
        {
            if (iter1 == endIter1)
            {
                return iter2 == endIter2;
            }

            if (iter2 == endIter2)
            {
                return false;
            }

            if (!predicate(*iter1, *iter2))
            {
                return false;
            }

            ++iter1;
            ++iter2;
        }
    }

    /// Compares two ranges defined by iterators for equality using the default equality operator.
    /// @tparam Iterator1 The type of the first iterator.
    /// @tparam Iterator2 The type of the second iterator.
    /// @param startIter1 The starting iterator of the first range.
    /// @param endIter1 The ending iterator of the first range (exclusive).
    /// @param startIter2 The starting iterator of the second range.
    /// @param endIter2 The ending iterator of the second range (exclusive).
    /// @return `true` if the elements in both ranges are equal according to the default
    template <typename Iterator1, typename Iterator2>
    constexpr bool equal(
        Iterator1 const startIter1,
        Iterator1 const endIter1,
        Iterator2 const startIter2,
        Iterator2 const endIter2)
    {
        return equal(startIter1, endIter1, startIter2, endIter2, EqualTo<>{});
    }

    /// Performs lexicographical comparison.
    template <typename Iterator1, typename Iterator2, typename Predicate>
    inline bool lexicographicalCompare(Iterator1 startIter1, Iterator1 endIter1, Iterator2 startIter2, Iterator2 endIter2, Predicate predicate)
    {
        for (; startIter1 != endIter1 && startIter2 != endIter2; ++startIter1, ++startIter2)
        {
            if (predicate(*startIter1, *startIter2))
            {
                return (true);
            }
            else if (predicate(*startIter2, *startIter1))
            {
                return (false);
            }
        }

        return (startIter1 == endIter1 && startIter2 != endIter2);
    }

    /// Copies elements from one range defined by two iterators to another range defined by an output iterator.
    /// @tparam InputIterator The type of the input iterator used to traverse the source range.
    /// @tparam OutputIterator The type of the output iterator used to store the copied elements.
    /// @param startIter The starting iterator of the source range.
    /// @param endIter The ending iterator of the source range (exclusive).
    /// @param outIter The output iterator where the copied elements will be stored.
    /// @return An iterator pointing to the end of the copied range in the output iterator.
    /// Copies elements from one range to another.
    template <typename InputIterator, typename OutputIterator>
    constexpr OutputIterator copy(InputIterator startIter, InputIterator endIter, OutputIterator outIter)
    {
        for (; startIter != endIter; ++outIter, ++startIter)
        {
            *outIter = *startIter;
        }

        return outIter;
    }

    /// Reverses the element order.
    template <typename Iterator>
    constexpr void reverse(Iterator startIter, Iterator endIter)
    {
        for (; startIter != endIter && startIter != --endIter; ++startIter)
        {
            Ark::swap(*startIter, *endIter);
        }
    }

    /// Searches for a specific value within a range defined by two iterators.
    /// @tparam Iterator The type of the iterator used to traverse the range.
    /// @tparam Type The type of the value being searched for.
    /// @param startIter The starting iterator of the range to search.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param value The value to search for in the range.
    /// @return An iterator pointing to the first occurrence of the value, or `endIter` if the value is not found.
    /// Finds an element.
    template <typename Iterator, typename Type>
    constexpr Iterator find(Iterator startIter, Iterator const endIter, Type const& value)
    {
        if constexpr (sizeof(Iterator) == 1)
        {
            const auto startIterPtr = static_cast<Iterator*>(startIter);
            const auto result = static_cast<Traits::Internal::RemoveReference<Iterator>*>(
                Memory::find(startIterPtr, static_cast<byte>(value), static_cast<usize>(endIter - startIter)));

            if constexpr (Traits::isPointer<Iterator>)
            {
                return result ? result : endIter;
            }
            else
            {
                return result ? startIter + (result - startIterPtr) : endIter;
            }
        }

        for (; startIter != endIter; ++startIter)
        {
            if (*startIter == value)
            {
                break;
            }
        }

        return startIter;
    }

    /// Searches for the first element in a range that satisfies a given predicate.
    /// @tparam Iterator The type of the iterator used to traverse the range.
    /// @tparam Predicate The type of the predicate used to check elements.
    /// @param startIter The starting iterator of the range to search.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param predicate The predicate to apply to each element in the range.
    /// @return An iterator pointing to the first element that satisfies the predicate, or `endIter` if no such element is found.
    /// Finds if.
    template <typename Iterator, typename Predicate>
    constexpr Iterator findIf(Iterator startIter, Iterator const endIter, Predicate predicate)
    {
        for (; startIter != endIter; ++startIter)
        {
            if (predicate(*startIter))
            {
                break;
            }
        }

        return startIter;
    }

    // Checks if any element in a range satisfies a given predicate.
    // @tparam Iterator The type of the iterator used to traverse the range.
    // @tparam Predicate The type of the predicate used to check elements.
    // @param startIter The starting iterator of the range to check.
    // @param endIter The ending iterator of the range (exclusive).
    // @param predicate The predicate to apply to each element in the range.
    // @return `true` if any element in the range satisfies the predicate, otherwise `false`.
    /// Returns whether any element matches the predicate.
    template <typename Iterator, typename Predicate>
    constexpr bool anyOf(Iterator const startIter, Iterator const endIter, Predicate predicate)
    {
        Iterator iter = startIter;

        for (; iter != endIter; ++iter)
        {
            if (predicate(*iter))
            {
                return true;
            }
        }

        return false;
    }

    // Checks if all elements in a range satisfy a given predicate.
    // @tparam Iterator The type of the iterator used to traverse the range.
    // @tparam Predicate The type of the predicate used to check elements.
    // @param startIter The starting iterator of the range to check.
    // @param endIter The ending iterator of the range (exclusive).
    // @param predicate The predicate to apply to each element in the range.
    // @return `true` if all elements in the range satisfy the predicate, otherwise `false`.
    /// Returns whether all elements match the predicate.
    template <typename Iterator, typename Predicate>
    constexpr bool allOf(Iterator startIter, Iterator endIter, Predicate predicate)
    {
        Iterator iter = startIter;

        for (; iter != endIter; ++iter)
        {
            if (!predicate(*iter))
            {
                return false;
            }
        }

        return true;
    }

    namespace Internal
    {
        template <typename Iterator, typename Compare>
        constexpr void insertionSort(Iterator startIter, Iterator endIter, Compare compare)
        {
            if (startIter == endIter)
            {
                return;
            }

            for (Iterator i = startIter + 1; i != endIter; ++i)
            {
                auto value = *i;
                Iterator j = i;
                while (j != startIter)
                {
                    Iterator prev = j;
                    --prev;
                    if (!compare(value, *prev))
                    {
                        break;
                    }
                    *j = *prev;
                    j = prev;
                }
                *j = value;
            }
        }

        template <typename Iterator, typename Compare>
        constexpr Iterator partition(Iterator startIter, Iterator endIter, Compare compare)
        {
            Iterator pivotIter = endIter;
            --pivotIter;
            auto const& pivot = *pivotIter;
            Iterator store = startIter;

            for (Iterator i = startIter; i != pivotIter; ++i)
            {
                if (compare(*i, pivot))
                {
                    Ark::swap(*i, *store);
                    ++store;
                }
            }

            Ark::swap(*store, *pivotIter);
            return store;
        }

        template <typename Iterator, typename Compare>
        constexpr void quickSort(Iterator startIter, Iterator endIter, Compare compare)
        {
            auto const count = endIter - startIter;
            if (count <= 1)
            {
                return;
            }

            if (count < 32)
            {
                insertionSort(startIter, endIter, compare);
                return;
            }

            Iterator mid = partition(startIter, endIter, compare);
            quickSort(startIter, mid, compare);
            if (mid != endIter)
            {
                quickSort(mid + 1, endIter, compare);
            }
        }
    }

    /// Sorts elements in a range in ascending order.
    /// @tparam Iterator The type of the iterator.
    /// @param startIter The starting iterator of the range to sort.
    /// @param endIter The ending iterator of the range (exclusive).
    /// Sorts the range in ascending order.
    template <typename Iterator>
    inline void sort(Iterator startIter, Iterator endIter)
    {
        Internal::quickSort(startIter, endIter, Less<>{});
    }

    /// Sorts elements in a range using a custom comparison function.
    /// @tparam Iterator The type of the iterator.
    /// @tparam Compare The type of the comparison function.
    /// @param startIter The starting iterator of the range to sort.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param compare The comparison function.
    /// Sorts the range in ascending order.
    template <typename Iterator, typename Compare>
    inline void sort(Iterator startIter, Iterator endIter, Compare compare)
    {
        Internal::quickSort(startIter, endIter, compare);
    }

    /// Sorts elements in a range while preserving the relative order of equal elements.
    /// @tparam Iterator The type of the iterator.
    /// @param startIter The starting iterator of the range to sort.
    /// @param endIter The ending iterator of the range (exclusive).
    /// Sorts the range while preserving equivalent element order.
    template <typename Iterator>
    inline void stableSort(Iterator startIter, Iterator endIter)
    {
        Internal::insertionSort(startIter, endIter, Less<>{});
    }

    /// Sorts elements in a range using a custom comparison function while preserving the relative order of equal elements.
    /// @tparam Iterator The type of the iterator.
    /// @tparam Compare The type of the comparison function.
    /// @param startIter The starting iterator of the range to sort.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param compare The comparison function.
    /// Sorts the range while preserving equivalent element order.
    template <typename Iterator, typename Compare>
    inline void stableSort(Iterator startIter, Iterator endIter, Compare compare)
    {
        Internal::insertionSort(startIter, endIter, compare);
    }

    /// Concept that checks if a type has getStartIterator() and getEndIterator() methods.
    /// @tparam T The type to check.
    /// Constrains this function with requires clauses.
    template <typename T>
    concept Iterable = requires(T& collection) {
        { collection.getStartIterator() };
        { collection.getEndIterator() };
    };

    /// Sorts elements in a collection in ascending order.
    /// @tparam Collection The type of the collection.
    /// @param collection The collection to sort.
    /// Sorts the range in ascending order.
    template <Iterable Collection>
    inline void sort(Collection& collection)
    {
        sort(collection.getStartIterator(), collection.getEndIterator());
    }

    /// Sorts elements in a collection using a custom comparison function.
    /// @tparam Collection The type of the collection.
    /// @tparam Compare The type of the comparison function.
    /// @param collection The collection to sort.
    /// @param compare The comparison function.
    /// Sorts the range in ascending order.
    template <Iterable Collection, typename Compare>
    inline void sort(Collection& collection, Compare compare)
    {
        sort(collection.getStartIterator(), collection.getEndIterator(), compare);
    }

    /// Sorts elements in a collection while preserving the relative order of equal elements.
    /// @tparam Collection The type of the collection.
    /// @param collection The collection to sort.
    /// Sorts the range while preserving equivalent element order.
    template <Iterable Collection>
    inline void stableSort(Collection& collection)
    {
        stableSort(collection.getStartIterator(), collection.getEndIterator());
    }

    /// Sorts elements in a collection using a custom comparison function while preserving the relative order of equal elements.
    /// @tparam Collection The type of the collection.
    /// @tparam Compare The type of the comparison function.
    /// @param collection The collection to sort.
    /// @param compare The comparison function.
    /// Sorts the range while preserving equivalent element order.
    template <Iterable Collection, typename Compare>
    inline void stableSort(Collection& collection, Compare compare)
    {
        stableSort(collection.getStartIterator(), collection.getEndIterator(), compare);
    }

    /// Returns the smaller of two values.
    /// @tparam T The type of the values.
    /// @param a The first value.
    /// @param b The second value.
    /// @return The smaller value.
    /// Returns the smaller value.
    template <typename T>
    constexpr T const& min(T const& a, T const& b)
    {
        return b < a ? b : a;
    }

    /// Returns the smaller of two values using a custom comparison function.
    /// @tparam T The type of the values.
    /// @tparam Compare The type of the comparison function.
    /// @param a The first value.
    /// @param b The second value.
    /// @param compare The comparison function.
    /// @return The smaller value.
    /// Returns the smaller value.
    template <typename T, typename Compare>
    constexpr T const& min(T const& a, T const& b, Compare compare)
    {
        return compare(b, a) ? b : a;
    }

    /// Returns the larger of two values.
    /// @tparam T The type of the values.
    /// @param a The first value.
    /// @param b The second value.
    /// @return The larger value.
    /// Returns the larger value.
    template <typename T>
    constexpr T const& max(T const& a, T const& b)
    {
        return a < b ? b : a;
    }

    /// Returns the larger of two values using a custom comparison function.
    /// @tparam T The type of the values.
    /// @tparam Compare The type of the comparison function.
    /// @param a The first value.
    /// @param b The second value.
    /// @param compare The comparison function.
    /// @return The larger value.
    /// Returns the larger value.
    template <typename T, typename Compare>
    constexpr T const& max(T const& a, T const& b, Compare compare)
    {
        return compare(a, b) ? b : a;
    }

    /// Clamps a value between a minimum and maximum.
    /// @tparam T The type of the value.
    /// @param value The value to clamp.
    /// @param minValue The minimum value.
    /// @param maxValue The maximum value.
    /// @return The clamped value.
    /// Clamps a value to the specified bounds.
    template <typename T>
    constexpr T const& clamp(T const& value, T const& minValue, T const& maxValue)
    {
        return value < minValue ? minValue : (maxValue < value ? maxValue : value);
    }

    /// Clamps a value between a minimum and maximum using a custom comparison function.
    /// @tparam T The type of the value.
    /// @tparam Compare The type of the comparison function.
    /// @param value The value to clamp.
    /// @param minValue The minimum value.
    /// @param maxValue The maximum value.
    /// @param compare The comparison function.
    /// @return The clamped value.
    /// Clamps a value to the specified bounds.
    template <typename T, typename Compare>
    constexpr T const& clamp(T const& value, T const& minValue, T const& maxValue, Compare compare)
    {
        return compare(value, minValue) ? minValue : (compare(maxValue, value) ? maxValue : value);
    }

    /// Finds the minimum element in a range.
    /// @tparam Iterator The type of the iterator.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @return An iterator pointing to the minimum element.
    /// Returns an iterator to the smallest element.
    template <typename Iterator>
    constexpr Iterator minElement(Iterator startIter, Iterator endIter)
    {
        return minElement(startIter, endIter, Less<>{});
    }

    /// Finds the minimum element in a range using a custom comparison function.
    /// @tparam Iterator The type of the iterator.
    /// @tparam Compare The type of the comparison function.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param compare The comparison function.
    /// @return An iterator pointing to the minimum element.
    /// Returns an iterator to the smallest element.
    template <typename Iterator, typename Compare>
    constexpr Iterator minElement(Iterator startIter, Iterator endIter, Compare compare)
    {
        if (startIter == endIter)
        {
            return endIter;
        }

        Iterator best = startIter;
        for (Iterator i = startIter; ++i != endIter;)
        {
            if (compare(*i, *best))
            {
                best = i;
            }
        }
        return best;
    }

    /// Finds the maximum element in a range.
    /// @tparam Iterator The type of the iterator.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @return An iterator pointing to the maximum element.
    /// Returns an iterator to the largest element.
    template <typename Iterator>
    constexpr Iterator maxElement(Iterator startIter, Iterator endIter)
    {
        return maxElement(startIter, endIter, Less<>{});
    }

    /// Finds the maximum element in a range using a custom comparison function.
    /// @tparam Iterator The type of the iterator.
    /// @tparam Compare The type of the comparison function.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param compare The comparison function.
    /// @return An iterator pointing to the maximum element.
    /// Returns an iterator to the largest element.
    template <typename Iterator, typename Compare>
    constexpr Iterator maxElement(Iterator startIter, Iterator endIter, Compare compare)
    {
        if (startIter == endIter)
        {
            return endIter;
        }

        Iterator best = startIter;
        for (Iterator i = startIter; ++i != endIter;)
        {
            if (compare(*best, *i))
            {
                best = i;
            }
        }
        return best;
    }

    /// Checks if a range is sorted in ascending order.
    /// @tparam Iterator The type of the iterator.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @return `true` if the range is sorted, otherwise `false`.
    /// Returns whether it is sorted.
    template <typename Iterator>
    constexpr bool isSorted(Iterator startIter, Iterator endIter)
    {
        return isSorted(startIter, endIter, Less<>{});
    }

    /// Checks if a range is sorted using a custom comparison function.
    /// @tparam Iterator The type of the iterator.
    /// @tparam Compare The type of the comparison function.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param compare The comparison function.
    /// @return `true` if the range is sorted, otherwise `false`.
    /// Returns whether it is sorted.
    template <typename Iterator, typename Compare>
    constexpr bool isSorted(Iterator startIter, Iterator endIter, Compare compare)
    {
        if (startIter == endIter)
        {
            return true;
        }

        Iterator next = startIter;
        ++next;
        for (; next != endIter; ++startIter, ++next)
        {
            if (compare(*next, *startIter))
            {
                return false;
            }
        }
        return true;
    }

    /// Fills a range with a specified value.
    /// @tparam Iterator The type of the iterator.
    /// @tparam T The type of the value.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param value The value to fill the range with.
    /// Assigns the same value to each element in the range.
    template <typename Iterator, typename T>
    constexpr void fill(Iterator startIter, Iterator endIter, T const& value)
    {
        for (; startIter != endIter; ++startIter)
        {
            *startIter = value;
        }
    }

    /// Removes consecutive duplicate elements from a range.
    /// @tparam Iterator The type of the iterator.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @return An iterator pointing to the new end of the range.
    /// Removes consecutive duplicate elements.
    template <typename Iterator>
    inline Iterator unique(Iterator startIter, Iterator endIter)
    {
        return unique(startIter, endIter, EqualTo<>{});
    }

    /// Removes consecutive duplicate elements from a range using a custom comparison function.
    /// @tparam Iterator The type of the iterator.
    /// @tparam BinaryPredicate The type of the comparison function.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param predicate The comparison function.
    /// @return An iterator pointing to the new end of the range.
    /// Removes consecutive duplicate elements.
    template <typename Iterator, typename BinaryPredicate>
    inline Iterator unique(Iterator startIter, Iterator endIter, BinaryPredicate predicate)
    {
        if (startIter == endIter)
        {
            return endIter;
        }

        Iterator result = startIter;
        while (++startIter != endIter)
        {
            if (!predicate(*result, *startIter))
            {
                ++result;
                if (result != startIter)
                {
                    *result = *startIter;
                }
            }
        }
        return ++result;
    }

    /// Finds the first position where a value could be inserted to maintain sorted order.
    /// @tparam Iterator The type of the iterator.
    /// @tparam T The type of the value.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param value The value to find the position for.
    /// @return An iterator pointing to the position.
    /// Returns the first element not less than the provided key.
    template <typename Iterator, typename T>
    constexpr Iterator lowerBound(Iterator startIter, Iterator endIter, T const& value)
    {
        return lowerBound(startIter, endIter, value, Less<>{});
    }

    /// Finds the first position where a value could be inserted to maintain sorted order using a custom comparison function.
    /// @tparam Iterator The type of the iterator.
    /// @tparam T The type of the value.
    /// @tparam Compare The type of the comparison function.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param value The value to find the position for.
    /// @param compare The comparison function.
    /// @return An iterator pointing to the position.
    /// Returns the first element not less than the provided key.
    template <typename Iterator, typename T, typename Compare>
    constexpr Iterator lowerBound(Iterator startIter, Iterator endIter, T const& value, Compare compare)
    {
        auto count = endIter - startIter;
        while (count > 0)
        {
            auto const step = count / 2;
            Iterator mid = startIter + step;
            if (compare(*mid, value))
            {
                startIter = ++mid;
                count -= step + 1;
            }
            else
            {
                count = step;
            }
        }
        return startIter;
    }

    /// Finds the last position where a value could be inserted to maintain sorted order.
    /// @tparam Iterator The type of the iterator.
    /// @tparam T The type of the value.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param value The value to find the position for.
    /// @return An iterator pointing to the position.
    /// Returns the first element greater than the provided key.
    template <typename Iterator, typename T>
    constexpr Iterator upperBound(Iterator startIter, Iterator endIter, T const& value)
    {
        return upperBound(startIter, endIter, value, Less<>{});
    }

    /// Finds the last position where a value could be inserted to maintain sorted order using a custom comparison function.
    /// @tparam Iterator The type of the iterator.
    /// @tparam T The type of the value.
    /// @tparam Compare The type of the comparison function.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param value The value to find the position for.
    /// @param compare The comparison function.
    /// @return An iterator pointing to the position.
    /// Returns the first element greater than the provided key.
    template <typename Iterator, typename T, typename Compare>
    constexpr Iterator upperBound(Iterator startIter, Iterator endIter, T const& value, Compare compare)
    {
        auto count = endIter - startIter;
        while (count > 0)
        {
            auto const step = count / 2;
            Iterator mid = startIter + step;
            if (!compare(value, *mid))
            {
                startIter = ++mid;
                count -= step + 1;
            }
            else
            {
                count = step;
            }
        }
        return startIter;
    }

    /// Performs a binary search on a sorted range.
    /// @tparam Iterator The type of the iterator.
    /// @tparam T The type of the value to search for.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param value The value to search for.
    /// @return `true` if the value is found, otherwise `false`.
    /// Performs a binary search on a sorted range.
    template <typename Iterator, typename T>
    constexpr bool binarySearch(Iterator startIter, Iterator endIter, T const& value)
    {
        return binarySearch(startIter, endIter, value, Less<>{});
    }

    /// Performs a binary search on a sorted range.
    /// @tparam Iterator The type of the iterator.
    /// @tparam T The type of the value to search for.
    /// @tparam Compare The type of the comparison function.
    /// @param startIter The starting iterator of the range.
    /// @param endIter The ending iterator of the range (exclusive).
    /// @param value The value to search for.
    /// @param compare The comparison function.
    /// @return `true` if the value is found, otherwise `false`.
    /// Performs a binary search on a sorted range.
    template <typename Iterator, typename T, typename Compare>
    constexpr bool binarySearch(Iterator startIter, Iterator endIter, T const& value, Compare compare)
    {
        Iterator it = lowerBound(startIter, endIter, value, compare);
        return it != endIter && !compare(value, *it);
    }
}
