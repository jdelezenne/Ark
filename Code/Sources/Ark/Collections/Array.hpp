#pragma once

#include "Ark/Collections/Algorithms.hpp"
#include "Ark/Collections/ArrayPolicy.hpp"
#include "Ark/Collections/Internal/RandomAccessIterator.hpp"
#include "Ark/Collections/Internal/ReverseIterator.hpp"
#include "Ark/Collections/Slice.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Memory/Functions.hpp"
#include "Ark/Memory/Memory.hpp"

#include <new>

namespace Ark::Collections
{
    /// A dynamic array implementation.
    /// @tparam T The type of elements in the array.
    /// @tparam Policy The policy for the array.
    template <typename T, ArrayPolicy Policy = ArrayPolicyType>
    struct Array final
    {
#pragma region Types

    public:
        /// The type of the collection.
        using CollectionType = Array;

        /// The type of the policy for the array.
        using PolicyType = Policy;

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
        Memory::Allocator& allocator = Memory::getDefaultAllocator();

        Pointer data{nullptr};
        SizeType count{0};
        SizeType capacity{0};

#pragma region Constructors and Assignments

    public:
        Array(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
        }

        explicit Array(SizeType count, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , capacity{count}
            , count{count}
        {
            if (count != 0)
            {
                data = static_cast<Pointer>(allocator.allocate(count * sizeof(T)));

                for (SizeType i = 0; i < count; ++i)
                {
                    new (&data[i]) T();
                }
            }
        }

        explicit Array(SizeType count, T const& value, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , capacity{count}
            , count{count}
        {
            if (count != 0)
            {
                data = static_cast<Pointer>(allocator.allocate(count * sizeof(T)));

                for (SizeType i = 0; i < count; ++i)
                {
                    new (&data[i]) T(value);
                }
            }
        }

        Array(InitializerList<T> initializerList)
            : capacity{initializerList.size()}
            , count{initializerList.size()}
        {
            data = static_cast<Pointer>(allocator.allocate(initializerList.size() * sizeof(T)));

            for (SizeType i = 0; i < count; ++i)
            {
                new (&data[i]) T(*(initializerList.begin() + i));
            }
        }

        /// Constructs an array.
        template <class Iterator>
        Array(Iterator startIter, Iterator endIter)
        {
            ARK_ASSERT(startIter <= endIter);

            capacity = static_cast<SizeType>(endIter - startIter);
            count = capacity;
            data = static_cast<Pointer>(allocator.allocate(capacity * sizeof(T)));

            for (SizeType i = 0; i < count; ++i)
            {
                new (&data[i]) T(*(startIter + i));
            }
        }

        Array(const Slice<T> slice)
            : capacity{slice.getCount()}
            , count{slice.getCount()}
        {
            data = static_cast<Pointer>(allocator.allocate(slice.getCount() * sizeof(T)));

            for (SizeType i = 0; i < count; ++i)
            {
                new (&data[i]) T(slice[i]);
            }
        }

        Array(ConstPointer start, ConstPointer end)
            : allocator{Memory::getDefaultAllocator()}
        {
            ARK_ASSERT(start <= end);

            capacity = static_cast<SizeType>(end - start);
            count = capacity;
            data = static_cast<Pointer>(allocator.allocate(capacity * sizeof(T)));

            for (SizeType i = 0; i < count; ++i)
            {
                new (&data[i]) T(*(start + i));
            }
        }

        Array(const Array& other)
            : capacity{other.capacity}
            , count{other.count}
        {
            data = static_cast<Pointer>(allocator.allocate(other.capacity * sizeof(T)));

            for (SizeType i = 0; i < count; ++i)
            {
                new (&data[i]) T(other.data[i]);
            }
        }

        ~Array()
        {
            clear();
            if (data != nullptr)
            {
                allocator.deallocate(data);
            }
        }

        // Reserves enough space to store the specified number of elements.
        /// Constructs with the requested capacity.
        static Array withCapacity(SizeType capacity, Memory::Allocator& allocator)
        {
            Array result = Array(allocator);
            result.reserve(capacity);
            return result;
        }

        Array& operator=(const Array& other)
        {
            if (this != &other)
            {
                clear();

                if (capacity < other.count)
                {
                    if (data != nullptr)
                    {
                        allocator.deallocate(data);
                    }
                    data = static_cast<Pointer>(allocator.allocate(other.capacity * sizeof(T)));
                    capacity = other.capacity;
                }

                count = other.count;

                for (SizeType i = 0; i < count; ++i)
                {
                    new (&data[i]) T(other.data[i]);
                }
            }

            return *this;
        }

        Array(Array&& other)
            : allocator{other.allocator}
            , data{other.data}
            , count{other.count}
            , capacity{other.capacity}
        {
            other.data = nullptr;
            other.count = 0;
            other.capacity = 0;
        }

        Array& operator=(Array&& other)
        {
            if (this != &other)
            {
                clear();
                if (data != nullptr)
                {
                    allocator.deallocate(data);
                }

                if (&allocator == &other.allocator)
                {
                    data = other.data;
                    count = other.count;
                    capacity = other.capacity;
                    other.data = nullptr;
                    other.count = 0;
                    other.capacity = 0;
                }
                else
                {
                    if (other.count == 0)
                    {
                        data = nullptr;
                        count = 0;
                        capacity = 0;
                    }
                    else
                    {
                        data = static_cast<Pointer>(allocator.allocate(other.count * sizeof(T)));
                        capacity = other.count;
                        count = other.count;

                        for (SizeType i = 0; i < count; ++i)
                        {
                            new (&data[i]) T(Ark::move(other.data[i]));
                            other.data[i].~T();
                        }

                        other.count = 0;
                    }
                }
            }
            return *this;
        }

#pragma endregion

#pragma region Iterators

    public:
        /// The iterator for the array.
        using Iterator = Internal::RandomAccessIterator<Array>;

        /// The const iterator for the array.
        using ConstIterator = Internal::ConstRandomAccessIterator<Array>;

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

        /// Gets an iterator to the end of the array.
        /// @return An iterator to the end of the array.
        constexpr Iterator getEndIterator()
        {
            return Iterator(data + count);
        }

        /// Gets a const iterator to the end of the array.
        /// @return A const iterator to the end of the array.
        constexpr ConstIterator getEndIterator() const
        {
            return ConstIterator(data + count);
        }

        /// Gets a reverse iterator to the end of the array.
        /// @return A reverse iterator to the end of the array.
        constexpr ReverseIterator getStartReverseIterator()
        {
            return ReverseIterator(getEndIterator());
        }

        /// Gets a const reverse iterator to the end of the array.
        /// @return A const reverse iterator to the end of the array.
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

    public:
        /// Gets the current capacity of the array.
        /// @return The current capacity of the array.
        constexpr SizeType getCapacity() const
        {
            return capacity;
        }

        /// Gets the number of elements in the array.
        /// @return The number of elements in the array.
        constexpr SizeType getCount() const
        {
            return count;
        }

        /// Gets the number of bytes in the array.
        /// @return The number of bytes in the array.
        constexpr SizeType getByteSize() const
        {
            return count * sizeof(T);
        }

        /// Checks if the array is empty.
        /// @return `true` if the array is empty, otherwise `false`.
        constexpr bool isEmpty() const
        {
            return count == 0;
        }

        /// Gets a pointer to the underlying data.
        /// @return A pointer to the underlying data.
        constexpr Pointer asMutablePointer()
        {
            return data;
        }

        /// Gets a const pointer to the underlying data.
        /// @return A const pointer to the underlying data.
        constexpr ConstPointer asPointer() const
        {
            return data;
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

            return data[count - 1];
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

    public:
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

#pragma endregion

#pragma region Convenience Modifiers

    public:
        /// Clears the contents of the array.
        void removeAll()
        {
            for (SizeType i = 0; i < count; ++i)
            {
                data[i].~T();
            }

            count = 0;
        }

        /// Removes all elements.
        void clear()
        {
            removeAll();
        }

        /// Adds an element to the end of the array.
        /// @param value The value to add.
        void append(T const& value)
        {
            if (count == capacity)
            {
                grow();
            }

            new (&data[count]) T(value);
            count++;
        }

        /// Adds an element to the end of the array.
        /// @param value The value to add.
        void append(T&& value)
        {
            if (count == capacity)
            {
                grow();
            }

            new (&data[count]) T(Ark::move(value));
            count++;
        }

        /// Constructs and appends an element.
        template <typename... Args>
        Reference appendMake(Args&&... args)
        {
            if (count == capacity)
            {
                grow();
            }

            new (&data[count]) T(Ark::forward<Args>(args)...);
            count++;

            return data[count - 1];
        }

        /// Appends elements.
        void append(InitializerList<T> initializerList)
        {
            reserve(count + initializerList.size());

            for (SizeType i = 0; i < initializerList.size(); ++i)
            {
                new (&data[count + i]) T(*(initializerList.begin() + i));
            }

            count += initializerList.size();
        }

        /// Appends elements.
        void append(const Slice<T> slice)
        {
            auto const* slicePointer = slice.asPointer();
            auto const* dataStart = data;
            auto const* dataEnd = data + count;
            const bool overlapsSelf =
                slicePointer != nullptr &&
                data != nullptr &&
                slicePointer >= dataStart &&
                slicePointer < dataEnd;

            if (overlapsSelf)
            {
                Array copy(slice);
                reserve(count + copy.getCount());

                for (SizeType i = 0; i < copy.getCount(); ++i)
                {
                    new (&data[count + i]) T(Ark::move(copy[i]));
                }

                count += copy.getCount();
                return;
            }

            reserve(count + slice.getCount());

            for (SizeType i = 0; i < slice.getCount(); ++i)
            {
                new (&data[count + i]) T(slice[i]);
            }

            count += slice.getCount();
        }

        /// Appends elements.
        void append(ConstIterator start, ConstIterator end)
        {
            ARK_ASSERT(start <= end);

            const SizeType rangeCount = distance(start, end);
            reserve(count + rangeCount);

            for (SizeType i = 0; start != end; ++start, ++i)
            {
                new (&data[count + i]) T(*start);
            }

            count += rangeCount;
        }

        /// Inserts an element at the specified position.
        /// @param position The iterator to the position to insert at.
        /// @param value The value to insert.
        /// @return Iterator to the inserted value.
        Iterator insert(ConstIterator position, T const& value)
        {
            usize index = Ark::Collections::distance(ConstIterator(data), position);

            return insert(index, value);
        }

        /// Inserts an element at the specified position.
        /// @param position The iterator to the position to insert at.
        /// @param value The value to insert.
        /// @return Iterator to the inserted value.
        Iterator insert(ConstIterator position, T&& value)
        {
            usize index = Ark::Collections::distance(ConstIterator(data), position);

            return insert(index, Ark::move(value));
        }

        /// Inserts make.
        template <typename... Args>
        Reference insertMake(ConstIterator position, Args&&... args)
        {
            return insert(position, Ark::forward<Args>(args)...);
        }

        /// Inserts an element at the specified index.
        /// @param index The index of the element to insert.
        /// @param value The value to insert.
        /// @return Iterator to the inserted value.
        Iterator insert(usize index, T const& value)
        {
            ARK_ASSERT(index <= count);

            if (count == capacity)
            {
                grow();
            }

            if (index < count)
            {
                for (usize i = count; i > index; --i)
                {
                    new (&data[i]) T(data[i - 1]);
                    data[i - 1].~T();
                }
            }

            new (&data[index]) T(value);
            count++;

            return Iterator(&data[index]);
        }

        /// Inserts an element at the specified index.
        /// @param index The index of the element to insert.
        /// @param value The value to insert.
        /// @return Iterator to the inserted value.
        Iterator insert(usize index, T&& value)
        {
            ARK_ASSERT(index <= count);

            if (count == capacity)
            {
                grow();
            }

            if (index < count)
            {
                for (usize i = count; i > index; --i)
                {
                    new (&data[i]) T(Ark::move(data[i - 1]));
                    data[i - 1].~T();
                }
            }

            new (&data[index]) T(Ark::move(value));
            count++;

            return Iterator(&data[index]);
        }

        /// Inserts make.
        template <typename... Args>
        Reference insertMake(usize index, Args&&... args)
        {
            ARK_ASSERT(index <= count);

            if (count == capacity)
            {
                grow();
            }

            if (index < count)
            {
                for (usize i = count; i > index; --i)
                {
                    new (&data[i]) T(Ark::move(data[i - 1]));
                    data[i - 1].~T();
                }
            }

            new (&data[index]) T(Ark::forward<Args>(args)...);
            count++;

            return data[index];
        }

        /// Removes and returns the last element of the array.
        void removeLast()
        {
            ARK_ASSERT(count > 0);

            data[count - 1].~T();

            shrink(1);
        }

        /// Removes the element at the specified iterator.
        /// @param iter The iterator to the element to remove.
        /// @return `true` if the element was removed, otherwise `false`.
        bool removeAt(Iterator iter)
        {
            if (iter == getEndIterator())
            {
                return false;
            }

            return removeAt(static_cast<SizeType>(Collections::distance(getStartIterator(), iter)));
        }

        /// Removes the element at the specified index.
        /// @param index The index of the element to remove.
        /// @return `true` if the element was removed, otherwise `false`.
        bool removeAt(usize index)
        {
            if (index >= count)
            {
                return false;
            }

            if (index < count - 1)
            {
                for (SizeType i = index; i < count - 1; ++i)
                {
                    data[i] = Ark::move(data[i + 1]);
                }
            }

            data[count - 1].~T();

            shrink(1);
            return true;
        }

        /// Removes the first occurrence of the specified value from the array.
        /// @param value The value to remove.
        /// @return `true` if the value was removed, otherwise `false`.
        bool remove(T const& value)
        {
            const auto& iter = Collections::find(getStartIterator(), getEndIterator(), value);

            if (iter == getEndIterator())
            {
                return false;
            }

            removeAt(iter);
            return true;
        }

        /// Removes all occurrences of the specified value from the array.
        /// @param value The value to remove.
        /// @return The number of elements removed.
        SizeType removeAll(T const& value)
        {
            SizeType removedCount = 0;
            SizeType writeIndex = 0;

            // Move all elements that don't match the value to the front
            for (SizeType readIndex = 0; readIndex < count; ++readIndex)
            {
                if (!(data[readIndex] == value))
                {
                    if (writeIndex != readIndex)
                    {
                        data[writeIndex] = Ark::move(data[readIndex]);
                    }
                    writeIndex++;
                }
                else
                {
                    removedCount++;
                }
            }

            // Destroy the removed elements
            for (SizeType i = writeIndex; i < count; ++i)
            {
                data[i].~T();
            }

            shrink(removedCount);
            return removedCount;
        }

        /// Removes the first element that satisfies the predicate.
        /// @tparam Predicate The type of the predicate function.
        /// @param predicate The predicate function to test elements.
        /// @return An Option containing the removed element if found, otherwise None.
        /// Removes if.
        template <typename Predicate>
        Option<T> removeIf(Predicate predicate)
        {
            for (auto iter = getStartIterator(); iter != getEndIterator(); ++iter)
            {
                if (predicate(*iter))
                {
                    T removed = move(*iter);
                    removeAt(iter);
                    return removed;
                }
            }

            return none;
        }

        /// Reserves space for a certain number of elements.
        /// @param newCapacity The new capacity to reserve.
        void reserve(SizeType newCapacity)
        {
            if (newCapacity > capacity)
            {
                reallocate(newCapacity);
            }
        }

        /// Resizes the array to a specific length.
        /// @param newSize The new length of the array.
        void resize(SizeType newSize)
        {
            if (newSize > capacity)
            {
                grow(newSize);
            }

            if (newSize > count)
            {
                for (SizeType i = count; i < newSize; ++i)
                {
                    new (&data[i]) T();
                }
            }
            else if (newSize < count)
            {
                for (SizeType i = newSize; i < count; ++i)
                {
                    data[i].~T();
                }
            }

            count = newSize;
        }

        /// Resizes the array to a specific length.
        /// @param newSize The new length of the array.
        void resize(SizeType newSize, T const& value)
        {
            SizeType oldCount = count;
            resize(newSize);

            if (newSize > oldCount)
            {
                for (SizeType i = oldCount; i < newSize; ++i)
                {
                    data[i] = value;
                }
            }
        }

        /// Shrinks the capacity of the array to fit its current size.
        void shrinkToFit()
        {
            shrinkExact(count);
        }

        /// Adds an element to the end of the array.
        /// @param value The value to add.
        void push(T const& value)
        {
            append(value);
        }

        /// Adds an element to the end of the array.
        /// @param value The value to add.
        void push(T&& value)
        {
            append(Ark::move(value));
        }

        /// Removes and returns the last element of the array.
        void pop()
        {
            removeLast();
        }

#pragma region Modifiers

        /// Inserts at.
        Iterator insertAt(SizeType index, T const& value)
        {
            return insert(index, value);
        }

        /// Inserts at.
        Iterator insertAt(SizeType index, T&& value)
        {
            return insert(index, Ark::move(value));
        }

        /// Removes first.
        void removeFirst()
        {
            ARK_ASSERT(count > 0);
            removeAt(0);
        }

        /// Removes subrange.
        void removeSubrange(SizeType start, SizeType end)
        {
            ARK_ASSERT(start <= end);
            ARK_ASSERT(end <= count);

            while (end > start)
            {
                removeAt(start);
                --end;
            }
        }

        /// Replaces a subrange of elements.
        void replaceSubrange(SizeType start, SizeType end, Slice<T const> replacement)
        {
            removeSubrange(start, end);

            for (SizeType i = 0; i < replacement.getCount(); ++i)
            {
                insert(start + i, replacement[i]);
            }
        }

        /// Replaces a subrange of elements.
        void replaceSubrange(SizeType start, SizeType end, InitializerList<T> replacement)
        {
            replaceSubrange(start, end, Slice<T const>(replacement.begin(), replacement.size()));
        }

#pragma endregion

#undef ARK_STATIC_WHEN_RESIZABLE

#pragma endregion

#pragma region Utilities

    public:
        /// Returns a slice view.
        Slice<T> asSlice()
        {
            return Slice<T>(data, count);
        }

        /// Returns a slice view.
        Slice<T const> asSlice() const
        {
            return Slice<T const>(data, count);
        }

        /// Fills the array with the specified value.
        /// @param value The value to fill the array with.
        constexpr void fill(T const& value)
        {
            for (SizeType i = 0; i < count; ++i)
            {
                data[i] = value;
            }
        }

        /// Reverses the element order.
        void reverse()
        {
            Collections::reverse(getStartIterator(), getEndIterator());
        }

        /// Splits at the specified index.
        Pair<Array, Array> splitAt(SizeType index) const
        {
            ARK_ASSERT(index <= count);

            Array first(index);
            Array second(count - index);

            Collections::copy(getStartIterator(), getStartIterator() + index, first.getStartIterator());
            Collections::copy(getStartIterator() + index, getEndIterator(), second.getStartIterator());

            return {first, second};
        }

        /// Checks if the array contains the specified value.
        /// @param value The value to search for.
        /// @return `true` if the value is found, otherwise `false`.
        constexpr bool contains(T const& value) const
        {
            return Collections::find(getStartIterator(), getEndIterator(), value) != getEndIterator();
        }

#pragma endregion

#pragma region Comparisons

        bool operator==(const Array& other) const
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

        bool operator!=(const Array& other) const
        requires Concepts::EqualityComparable<T>
        {
            return !(*this == other);
        }

#pragma endregion

#pragma region Private

    private:
        /// Increases capacity.
        void grow(SizeType additionalCount = 1)
        {
            const SizeType newCount = count + additionalCount;

            if (newCount <= capacity)
            {
                return;
            }

            SizeType newCapacity;

            if constexpr (Policy::GrowStrategy == ResizeStrategy::Ratio)
            {
                newCapacity = static_cast<SizeType>(capacity * Policy::GrowRatio);
            }
            else if constexpr (Policy::GrowStrategy == ResizeStrategy::Count)
            {
                newCapacity = capacity + Policy::GrowCount;
            }
            else
            {
                ARK_ASSERT_MSG(Policy::GrowStrategy != ResizeStrategy::None, "Array cannot grow.");
            }

            newCapacity = Ark::max(newCapacity, count + additionalCount);
            newCapacity = Ark::max(newCapacity, Policy::MinimumCapacity);

            if constexpr (Policy::Granularity > 0)
            {
                newCapacity = (newCapacity + Policy::Granularity - 1) / Policy::Granularity * Policy::Granularity;
            }

            reallocate(newCapacity);
        }

        /// Reallocates storage.
        void reallocate(SizeType newCapacity)
        {
            Pointer newData = static_cast<Pointer>(allocator.allocate(newCapacity * sizeof(T)));

            for (SizeType i = 0; i < count; ++i)
            {
                new (&newData[i]) T(Ark::move(data[i]));
                data[i].~T();
            }

            if (data != nullptr)
            {
                allocator.deallocate(data);
            }

            data = newData;
            capacity = newCapacity;
        }

        /// Reduces capacity.
        void shrink(SizeType countToShrink)
        {
            ARK_ASSERT(countToShrink <= count);

            if (countToShrink == 0)
            {
                return;
            }

            count = Ark::max(count - countToShrink, SizeType(0));
            shrinkExact(count);
        }

        /// Reduces capacity to an exact size.
        void shrinkExact(SizeType newCount)
        {
            if (newCount == 0)
            {
                clear();
                return;
            }

            bool shouldShrink = false;

            if constexpr (Policy::ShrinkStrategy == ResizeStrategy::Ratio)
            {
                float32 ratio = static_cast<float32>(newCount) / static_cast<float32>(capacity);
                shouldShrink = (ratio <= Policy::ShrinkRatio && count >= Policy::MinimumCapacity);
            }
            else if constexpr (Policy::ShrinkStrategy == ResizeStrategy::Count)
            {
                int32 diff = capacity - newCount;
                shouldShrink = (diff >= Policy::ShrinkCount && newCount >= Policy::MinimumCapacity);
            }

            if (shouldShrink)
            {
                SizeType newCapacity = newCount;

                if constexpr (Policy::Granularity > 0)
                {
                    newCapacity = (newCapacity + Policy::Granularity - 1) / Policy::Granularity * Policy::Granularity;
                }

                reallocate(newCapacity);
            }
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

        constexpr reference at(size_type index)
        {
            return get(index);
        }

        constexpr const_reference at(size_type index) const
        {
            return get(index);
        }

        void push_back(T const& value)
        {
            append(value);
        }

        void push_back(T&& value)
        {
            append(Ark::move(value));
        }

        template <typename... Args>
        reference emplace_back(Args&&... args)
        {
            return appendMake(Ark::forward<Args>(args)...);
        }

        void pop_back()
        {
            removeLast();
        }

        iterator erase(const_iterator position)
        {
            SizeType const index = static_cast<SizeType>(Collections::distance(getStartIterator(), position));
            if (index >= count)
            {
                return getEndIterator();
            }

            removeAt(index);
            return Iterator(data + index);
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            SizeType const start = static_cast<SizeType>(Collections::distance(getStartIterator(), first));
            SizeType end = static_cast<SizeType>(Collections::distance(getStartIterator(), last));
            if (start >= count)
            {
                return getEndIterator();
            }

            end = Ark::min(end, count);
            removeSubrange(start, end);
            return Iterator(data + start);
        }

#pragma endregion
    };

#pragma region Free Functions

    /// Returns an iterator to the first element.
    template <typename T, ArrayPolicy Policy>
    constexpr auto begin(Array<T, Policy>& array)
    {
        return array.getStartIterator();
    }

    /// Returns an iterator to one-past-the-last element.
    template <typename T, ArrayPolicy Policy>
    constexpr auto end(Array<T, Policy>& array)
    {
        return array.getEndIterator();
    }

    /// Returns an iterator to the first element.
    template <typename T, ArrayPolicy Policy>
    constexpr auto begin(Array<T, Policy> const& array)
    {
        return array.getStartIterator();
    }

    /// Returns an iterator to one-past-the-last element.
    template <typename T, ArrayPolicy Policy>
    constexpr auto end(Array<T, Policy> const& array)
    {
        return array.getEndIterator();
    }

#pragma endregion

    template <typename T, ArrayPolicy Policy = ArrayPolicyType>
    struct ArrayBuilder final
    {
    private:
        using Array = Array<T, Policy>;
        using SizeType = typename Array::SizeType;
        using Pointer = typename Array::Pointer;
        using ConstPointer = typename Array::ConstPointer;

    private:
        Array collection;

    public:
        ArrayBuilder() = default;

        ArrayBuilder(SizeType initialCapacity)
            : collection{initialCapacity}
        {
        }

        ArrayBuilder(InitializerList<T> initializerList)
            : collection{initializerList}
        {
        }

        ArrayBuilder(const Slice<T> slice)
            : collection{slice}
        {
        }

        ArrayBuilder(ConstPointer start, ConstPointer end)
            : collection{start, end}
        {
        }

    public:
        /// Builds a value from the provided arguments.
        Array build()
        {
            return collection;
        }

        operator Array()
        {
            return build();
        }

    public:
        /// Appends elements.
        ArrayBuilder& append(T const& value)
        {
            collection.append(value);

            return *this;
        }

        /// Appends elements.
        ArrayBuilder& append(T&& value)
        {
            collection.append(Ark::move(value));

            return *this;
        }

        /// Constructs and appends an element.
        template <typename... Args>
        ArrayBuilder& appendMake(Args&&... args)
        {
            collection.appendMake(Ark::forward<Args>(args)...);

            return *this;
        }

        /// Appends elements.
        ArrayBuilder& append(InitializerList<T> initializerList)
        {
            collection.append(initializerList);

            return *this;
        }

        /// Appends elements.
        ArrayBuilder& append(const Slice<T> slice)
        {
            collection.append(slice);

            return *this;
        }

        /// Appends elements.
        ArrayBuilder& append(Array::ConstIterator start, Array::ConstIterator end)
        {
            collection.append(start, end);

            return *this;
        }

        /// Inserts an element.
        ArrayBuilder& insert(Array::ConstIterator position, T const& value)
        {
            collection.insert(position, value);

            return *this;
        }

        /// Inserts an element.
        ArrayBuilder& insert(Array::ConstIterator position, T&& value)
        {
            collection.insert(position, Ark::move(value));

            return *this;
        }

        /// Inserts make.
        template <typename... Args>
        ArrayBuilder& insertMake(Array::ConstIterator position, Args&&... args)
        {
            collection.insertMake(position, Ark::forward<Args>(args)...);

            return *this;
        }

        /// Inserts an element.
        ArrayBuilder& insert(usize index, T const& value)
        {
            collection.insert(index, value);

            return *this;
        }

        /// Inserts an element.
        ArrayBuilder& insert(usize index, T&& value)
        {
            collection.insert(index, Ark::move(value));

            return *this;
        }

        /// Inserts make.
        template <typename... Args>
        ArrayBuilder& insertMake(usize index, Args&&... args)
        {
            collection.insertMake(index, Ark::forward<Args>(args)...);

            return *this;
        }
    };
}
