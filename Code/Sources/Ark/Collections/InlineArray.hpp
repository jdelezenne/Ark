#pragma once

#include "Ark/Collections/Algorithms.hpp"
#include "Ark/Collections/Internal/RandomAccessIterator.hpp"
#include "Ark/Collections/Internal/ReverseIterator.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark::Collections
{
    /// A inline array implementation.
    /// @tparam T The type of elements in the array.
    /// @tparam N The number of elements in the array.
    /// @remark This is a wrapper around a C-style array.
    template <typename T, usize N>
    struct InlineArray final
    {
#pragma region Types

    public:
        /// The type of the collection.
        using CollectionType = InlineArray;

        /// The type of values stored in the array.
        using ValueType = T;

        /// The type of references to values in the array.
        using Reference = ValueType&;

        /// The type of const references to values in the array.
        using ConstReference = const ValueType&;

        /// The type of pointers to values in the array.
        using Pointer = ValueType*;

        /// The type of const pointers to values in the array.
        using ConstPointer = const ValueType*;

        /// The type used for sizes and indices.
        using SizeType = usize;

        /// The type used for pointer differences.
        using DifferenceType = isize;

#pragma endregion

    private:
        // inline array capacity
        static constexpr usize capacity = N;

        // inline array data
        T data[capacity]{};

#pragma region Constructors and Assignments

    public:
        /// Default constructor.
        constexpr InlineArray() = default;

        /// Constructor that initializes all elements with a given value.
        /// @param value The value to initialize all elements with.
        constexpr explicit InlineArray(T const& value)
        {
            fill(value);
        }

        /// Constructor from initializer list.
        /// @param initializer The initializer list to initialize the array with.
        constexpr InlineArray(InitializerList<T> initializerList)
        {
            const usize size = initializerList.size();

            ARK_ASSERT(size <= capacity);

            usize i = 0;
            for (auto iter = initializerList.begin(); iter != initializerList.end() && i < capacity; ++iter)
            {
                data[i] = *iter;
                ++i;
            }

            for (; i < capacity; ++i)
            {
                data[i] = T();
            }
        }

        /// Constructor from a C-style array.
        /// @param arr The C-style array to initialize from.
        /// Constructs an inline array.
        template <usize M>
        constexpr explicit InlineArray(const T (&arr)[M])
        {
            static_assert(M <= capacity, "Source array is too large");

            usize i = 0;
            for (; i < M; ++i)
            {
                data[i] = arr[i];
            }

            for (; i < capacity; ++i)
            {
                data[i] = T();
            }
        }

        /// Constructor from an iterator range.
        /// @tparam Iterator The type of the input iterators.
        /// @param startIter The iterator to the startIter element of the range.
        /// @param endIter The iterator to one past the endIter element of the range.
        /// Constructs an inline array.
        template <typename Iterator>
        constexpr InlineArray(Iterator startIter, Iterator endIter)
        {
            usize i = 0;

            for (auto iter = startIter; iter != endIter && i < capacity; ++iter)
            {
                data[i] = *iter;
                ++i;
            }

            for (; i < capacity; ++i)
            {
                data[i] = T();
            }
        }

        /// Copy constructor.
        constexpr InlineArray(InlineArray const&) = default;

        /// Move constructor.
        constexpr InlineArray(InlineArray&&) = default;

        constexpr InlineArray& operator=(InlineArray const&) = default;

        constexpr InlineArray& operator=(InlineArray&&) = default;

        constexpr InlineArray& operator=(InitializerList<T> initializerList)
        {
            ARK_ASSERT(initializerList.size() <= capacity);

            usize i = 0;
            for (auto iter = initializerList.begin(); iter != initializerList.end() && i < capacity; ++iter)
            {
                data[i] = *iter;
                ++i;
            }

            for (; i < capacity; ++i)
            {
                data[i] = T();
            }

            return *this;
        }

#pragma endregion

#pragma region Iterators

    public:
        /// The iterator for the array.
        using Iterator = Internal::RandomAccessIterator<InlineArray>;

        /// The const iterator for the array.
        using ConstIterator = Internal::ConstRandomAccessIterator<InlineArray>;

        /// The reverse iterator for the array.
        using ReverseIterator = Internal::ReverseIterator<Iterator>;

        /// The const reverse iterator for the array.
        using ConstReverseIterator = Internal::ReverseIterator<ConstIterator>;

        /// Gets an iterator to the beginning of the array.
        /// @return An iterator to the beginning of the array.
        constexpr Iterator getStartIterator()
        {
            return Iterator(data);
        }

        /// Gets a const iterator to the beginning of the array.
        /// @return A const iterator to the beginning of the array.
        constexpr ConstIterator getStartIterator() const
        {
            return ConstIterator(data);
        }

        /// Gets an iterator to the endIter of the array.
        /// @return An iterator to the endIter of the array.
        constexpr Iterator getEndIterator()
        {
            return Iterator(data + capacity);
        }

        /// Gets a const iterator to the endIter of the array.
        /// @return A const iterator to the endIter of the array.
        constexpr ConstIterator getEndIterator() const
        {
            return ConstIterator(data + capacity);
        }

        /// Gets a reverse iterator to the endIter of the array.
        /// @return A reverse iterator to the endIter of the array.
        constexpr ReverseIterator getStartReverseIterator()
        {
            return ReverseIterator(getEndIterator());
        }

        /// Gets a const reverse iterator to the endIter of the array.
        /// @return A const reverse iterator to the endIter of the array.
        constexpr ConstReverseIterator getStartReverseIterator() const
        {
            return ConstReverseIterator(getEndIterator());
        }

        /// Gets a reverse iterator to the beginning of the array.
        /// @return A reverse iterator to the beginning of the array.
        constexpr ReverseIterator getEndReverseIterator()
        {
            return ReverseIterator(getStartIterator());
        }

        /// Gets a const reverse iterator to the beginning of the array.
        /// @return A const reverse iterator to the beginning of the array.
        constexpr ConstReverseIterator getEndReverseIterator() const
        {
            return ConstReverseIterator(getStartIterator());
        }

#pragma endregion

#pragma region Accessors

        /// Gets the maximum number of elements the array can hold.
        /// @return The maximum number of elements the array can hold.
        constexpr SizeType getCapacity() const
        {
            return capacity;
        }

        /// Gets the number of elements in the array.
        /// @return The number of elements in the array.
        constexpr SizeType getCount() const
        {
            return capacity;
        }

        /// Gets the number of bytes in the array.
        /// @return The number of bytes in the array.
        constexpr SizeType getByteCount() const
        {
            return capacity * sizeof(T);
        }

        /// Checks if the array is empty.
        /// @return `true` if the array is empty, otherwise `false`.
        constexpr bool isEmpty() const
        {
            return capacity == 0;
        }

        /// Gets a pointer to the underlying data.
        /// @return A pointer to the underlying data.
        constexpr Pointer asPointer()
        {
            return data;
        }

        /// Gets a const pointer to the underlying data.
        /// @return A const pointer to the underlying data.
        constexpr ConstPointer asPointer() const
        {
            return data;
        }

        /// Gets a reference to the startIter element.
        /// @return A reference to the startIter element.
        constexpr Reference getFirst()
        {
            return data[0];
        }

        /// Gets a const reference to the startIter element.
        /// @return A const reference to the startIter element.
        constexpr ConstReference getFirst() const
        {
            return data[0];
        }

        /// Gets a reference to the endIter element.
        /// @return A reference to the endIter element.
        constexpr Reference getLast()
        {
            return data[capacity - 1];
        }

        /// Gets a const reference to the endIter element.
        /// @return A const reference to the endIter element.
        constexpr ConstReference getLast() const
        {
            return data[capacity - 1];
        }

#pragma endregion

#pragma region Accessors

        /// Returns the element at the specified index.
        constexpr Reference at(SizeType index)
        {
            return get(index);
        }

        /// Returns the element at the specified index.
        constexpr ConstReference at(SizeType index) const
        {
            return get(index);
        }

#pragma endregion

#pragma region Operators

        constexpr Reference operator[](SizeType index)
        {
            ARK_ASSERT(index < capacity);

            return data[index];
        }

        constexpr ConstReference operator[](SizeType index) const
        {
            ARK_ASSERT(index < capacity);

            return data[index];
        }

        bool operator==(InlineArray const& other) const
        {
            return Collections::equal(
                getStartIterator(),
                getEndIterator(),
                other.getStartIterator(),
                other.getEndIterator());
        }

        bool operator!=(InlineArray const& other) const
        {
            return !(*this == other);
        }

        bool operator<(InlineArray const& other) const
        {
            return lexicographicalCompare(
                getStartIterator(),
                getEndIterator(),
                other.getStartIterator(),
                other.getEndIterator());
        }

        bool operator>(InlineArray const& other) const
        {
            return other < *this;
        }

        bool operator<=(InlineArray const& other) const
        {
            return !(other < *this);
        }

        bool operator>=(InlineArray const& other) const
        {
            return !(*this < other);
        }

#pragma endregion

#pragma region Utilities

        /// Returns a reference to the element at the given index, without bounds checking.
        /// @param index The index of the element to access.
        /// @return A reference to the element at the given index.
        constexpr Reference get(SizeType index)
        {
            ARK_ASSERT(index < N);

            return data[index];
        }

        /// Returns a const reference to the element at the given index, without bounds checking.
        /// @param index The index of the element to access.
        /// @return A const reference to the element at the given index.
        constexpr ConstReference get(SizeType index) const
        {
            ARK_ASSERT(index < N);

            return data[index];
        }

        /// Returns an Option containing a reference to the element at the given index.
        /// @param index The index of the element to access.
        /// @return An Option containing a reference to the element if the index is valid, None otherwise.
        constexpr Option<Reference> tryGet(SizeType index)
        {
            if (index < N)
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
            if (index < N)
            {
                return data[index];
            }

            return none;
        }

        /// Fills the array with a value.
        /// @param value The value to fill the array with.
        constexpr void fill(T const& value)
        {
            for (SizeType i = 0; i < N; ++i)
            {
                data[i] = value;
            }
        }

        /// Reverses the element order.
        void reverse()
        {
            Collections::reverse(getStartIterator(), getEndIterator());
        }

        /// Returns a new array with the elements in reverse order.
        /// @return A new array with the elements in reverse order.
        constexpr InlineArray reversed() const
        {
            InlineArray result;

            for (SizeType i = 0; i < N; ++i)
            {
                result[i] = data[N - 1 - i];
            }

            return result;
        }

        /// Returns a new array with the elements rotated left by the given amount.
        /// @param n The number of positions to rotate left.
        /// @return A new array with the elements rotated left.
        constexpr InlineArray rotateLeft(SizeType n) const
        {
            InlineArray result;

            n %= N;

            for (SizeType i = 0; i < N; ++i)
            {
                result[i] = data[(i + n) % N];
            }

            return result;
        }

        /// Returns a new array with the elements rotated right by the given amount.
        /// @param n The number of positions to rotate right.
        /// @return A new array with the elements rotated right.
        constexpr InlineArray rotateRight(SizeType n) const
        {
            InlineArray result;

            n %= N;
            for (SizeType i = 0; i < N; ++i)
            {
                result[i] = data[(i + N - n) % N];
            }

            return result;
        }

        /// Splits at the specified index.
        Pair<InlineArray, InlineArray> splitAt(SizeType index) const
        {
            ARK_ASSERT(index <= N);

            InlineArray first;
            InlineArray second;

            Collections::copy(getStartIterator(), getStartIterator() + index, first.getStartIterator());
            Collections::copy(getStartIterator() + index, getEndIterator(), second.getStartIterator());

            return {first, second};
        }

        /// Swaps the contents of this array with another.
        /// @param other The other array to swap with.
        constexpr void swap(InlineArray& other)
        {
            for (SizeType i = 0; i < N; ++i)
            {
                T temp = Ark::move(data[i]);
                data[i] = Ark::move(other.data[i]);
                other.data[i] = Ark::move(temp);
            }
        }

        /// Checks if the array contains the specified value.
        /// @param value The value to search for.
        /// @return `true` if the value is found, otherwise `false`.
        constexpr bool contains(T const& value) const
        {
            return Collections::find(getStartIterator(), getEndIterator(), value) != getEndIterator();
        }

#pragma endregion

#pragma region Operators

        bool operator==(InlineArray const& other) const
        requires Concepts::EqualityComparable<T>
        {
            if (getCount() != other.getCount())
            {
                return false;
            }

            for (usize i = 0; i < getCount(); ++i)
            {
                if (!(data[i] == other.data[i]))
                {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(InlineArray const& other) const
        requires Concepts::EqualityComparable<T>
        {
            return !(*this == other);
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

        constexpr iterator begin()
        {
            return getStartIterator();
        }

        constexpr const_iterator begin() const
        {
            return getStartIterator();
        }

        constexpr const_iterator cbegin() const
        {
            return getStartIterator();
        }

        constexpr iterator end()
        {
            return getEndIterator();
        }

        constexpr const_iterator end() const
        {
            return getEndIterator();
        }

        constexpr const_iterator cend() const
        {
            return getEndIterator();
        }

        constexpr size_type size() const
        {
            return getCount();
        }

        constexpr bool empty() const
        {
            return isEmpty();
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

#pragma endregion
    };

#pragma region Free Functions

    /// Returns an iterator to the first element.
    template <typename T, usize N>
    constexpr auto begin(InlineArray<T, N>& array)
    {
        return array.getStartIterator();
    }

    /// Returns an iterator to one-past-the-last element.
    template <typename T, usize N>
    constexpr auto end(InlineArray<T, N>& array)
    {
        return array.getEndIterator();
    }

    /// Returns an iterator to the first element.
    template <typename T, usize N>
    constexpr auto begin(InlineArray<T, N> const& array)
    {
        return array.getStartIterator();
    }

    /// Returns an iterator to one-past-the-last element.
    template <typename T, usize N>
    constexpr auto end(InlineArray<T, N> const& array)
    {
        return array.getEndIterator();
    }

#pragma endregion

    template <typename T, usize N>
    using FrozenStaticArray = InlineArray<T, N>;

    /// Creates a InlineArray from a C-style array (copy version).
    /// @tparam T The element type.
    /// @tparam N The array size.
    /// @param array The C-style array to copy from.
    /// @return A InlineArray containing copies of the elements.
    /// Creates an array from the provided values.
    template <typename T, usize N>
    constexpr InlineArray<Traits::RemoveCVType<T>, N> makeArray(T (&array)[N])
    {
        InlineArray<Traits::RemoveCVType<T>, N> result;

        for (usize i = 0; i < N; ++i)
        {
            result[i] = array[i];
        }

        return result;
    }

    /// Creates a InlineArray from a C-style array (move version).
    /// @tparam T The element type.
    /// @tparam N The array size.
    /// @param array The C-style array to move from.
    /// @return A InlineArray containing moved elements.
    /// Creates an array from the provided values.
    template <typename T, usize N>
    constexpr InlineArray<Traits::RemoveCVType<T>, N> makeArray(T (&&array)[N])
    {
        InlineArray<Traits::RemoveCVType<T>, N> result;

        for (usize i = 0; i < N; ++i)
        {
            result[i] = Ark::move(array[i]);
        }

        return result;
    }

    namespace Internal
    {
        template <typename T, usize N, usize... Dimensions>
        struct MultiStaticArray
        {
            using type = InlineArray<typename MultiStaticArray<T, Dimensions...>::type, N>;
        };

        template <typename T, usize N>
        struct MultiStaticArray<T, N>
        {
            using type = InlineArray<T, N>;
        };
    }

    template <typename T, usize... Dimensions>
    using MultiStaticArray = typename Internal::MultiStaticArray<T, Dimensions...>::type;
}
