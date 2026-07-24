#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

#include <new>

namespace Ark::Collections
{
    /// A fixed-capacity ring buffer with O(1) ends access.
    /// @details Prefer this name over `CircularArray`: Ark's `Array` is a growable linear buffer,
    /// while this type is a classic circular/ring buffer with fixed capacity.
    /// @tparam T Element type.
    template <typename T>
    struct CircularBuffer final
    {
    private:
        Memory::Allocator& allocator{Memory::getDefaultAllocator()};
        T* data{nullptr};
        usize capacity{0};
        usize head{0};
        usize count{0};

    public:
#pragma region Types

        using ValueType = T;
        using SizeType = usize;
        using DifferenceType = isize;
        using Reference = ValueType&;
        using ConstReference = ValueType const&;
        using Pointer = ValueType*;
        using ConstPointer = ValueType const*;
        using AllocatorType = Memory::Allocator*;

        struct Iterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = Pointer;
            using reference = Reference;

            CircularBuffer* owner{nullptr};
            SizeType offset{0};

            Iterator() = default;
            Iterator(CircularBuffer* o, SizeType off)
                : owner(o)
                , offset(off)
            {
            }

            reference operator*() const
            {
                return (*owner)[offset];
            }

            pointer operator->() const
            {
                return &(*owner)[offset];
            }

            Iterator& operator++()
            {
                ++offset;
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator copy = *this;
                ++(*this);
                return copy;
            }

            bool operator==(Iterator const& other) const
            {
                return owner == other.owner && offset == other.offset;
            }

            bool operator!=(Iterator const& other) const
            {
                return !(*this == other);
            }
        };

        struct ConstIterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = ConstPointer;
            using reference = ConstReference;

            CircularBuffer const* owner{nullptr};
            SizeType offset{0};

            ConstIterator() = default;
            ConstIterator(CircularBuffer const* o, SizeType off)
                : owner(o)
                , offset(off)
            {
            }

            ConstIterator(Iterator const& other)
                : owner(other.owner)
                , offset(other.offset)
            {
            }

            reference operator*() const
            {
                return (*owner)[offset];
            }

            pointer operator->() const
            {
                return &(*owner)[offset];
            }

            ConstIterator& operator++()
            {
                ++offset;
                return *this;
            }

            ConstIterator operator++(int)
            {
                ConstIterator copy = *this;
                ++(*this);
                return copy;
            }

            bool operator==(ConstIterator const& other) const
            {
                return owner == other.owner && offset == other.offset;
            }

            bool operator!=(ConstIterator const& other) const
            {
                return !(*this == other);
            }
        };

#pragma endregion

#pragma region Constructors and Assignments

        CircularBuffer(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
        }

        explicit CircularBuffer(SizeType capacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
            reserveCapacity(capacity);
        }

        CircularBuffer(InitializerList<T> initializerList, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
            reserveCapacity(initializerList.size());
            for (T const& value : initializerList)
            {
                append(value);
            }
        }

        CircularBuffer(CircularBuffer const& other)
            : allocator{other.allocator}
        {
            reserveCapacity(other.capacity);
            for (SizeType i = 0; i < other.count; ++i)
            {
                append(other[i]);
            }
        }

        CircularBuffer(CircularBuffer&& other)
            : allocator{other.allocator}
            , data{other.data}
            , capacity{other.capacity}
            , head{other.head}
            , count{other.count}
        {
            other.data = nullptr;
            other.capacity = 0;
            other.head = 0;
            other.count = 0;
        }

        ~CircularBuffer()
        {
            removeAll();
            if (data != nullptr)
            {
                allocator.deallocate(data);
            }
        }

        CircularBuffer& operator=(CircularBuffer const& other)
        {
            if (this != &other)
            {
                removeAll();
                if (capacity < other.count)
                {
                    if (data != nullptr)
                    {
                        allocator.deallocate(data);
                        data = nullptr;
                        capacity = 0;
                    }
                    reserveCapacity(other.capacity);
                }
                head = 0;
                for (SizeType i = 0; i < other.count; ++i)
                {
                    append(other[i]);
                }
            }
            return *this;
        }

        CircularBuffer& operator=(CircularBuffer&& other)
        {
            if (this != &other)
            {
                removeAll();
                if (data != nullptr)
                {
                    allocator.deallocate(data);
                }

                data = other.data;
                capacity = other.capacity;
                head = other.head;
                count = other.count;

                other.data = nullptr;
                other.capacity = 0;
                other.head = 0;
                other.count = 0;
            }
            return *this;
        }

        /// Constructs a buffer with the given fixed capacity.
        static CircularBuffer withCapacity(SizeType capacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
        {
            return CircularBuffer(capacity, allocator);
        }

#pragma endregion

#pragma region Accessors

        SizeType getCount() const
        {
            return count;
        }

        SizeType getCapacity() const
        {
            return capacity;
        }

        bool isEmpty() const
        {
            return count == 0;
        }

        bool isFull() const
        {
            return capacity == 0 || count == capacity;
        }

        Reference getFirst()
        {
            ARK_ASSERT(!isEmpty());
            return data[head];
        }

        ConstReference getFirst() const
        {
            ARK_ASSERT(!isEmpty());
            return data[head];
        }

        Reference getLast()
        {
            ARK_ASSERT(!isEmpty());
            return (*this)[count - 1];
        }

        ConstReference getLast() const
        {
            ARK_ASSERT(!isEmpty());
            return (*this)[count - 1];
        }

        Option<Reference> tryGetFirst()
        {
            if (isEmpty())
            {
                return none;
            }
            return data[head];
        }

        Option<ConstReference> tryGetFirst() const
        {
            if (isEmpty())
            {
                return none;
            }
            return data[head];
        }

        Option<Reference> tryGetLast()
        {
            if (isEmpty())
            {
                return none;
            }
            return (*this)[count - 1];
        }

        Option<ConstReference> tryGetLast() const
        {
            if (isEmpty())
            {
                return none;
            }
            return (*this)[count - 1];
        }

        Option<Reference> tryGet(SizeType index)
        {
            if (index >= count)
            {
                return none;
            }
            return (*this)[index];
        }

        Option<ConstReference> tryGet(SizeType index) const
        {
            if (index >= count)
            {
                return none;
            }
            return (*this)[index];
        }

        AllocatorType getAllocator() const
        {
            return &allocator;
        }

#pragma endregion

#pragma region Iterators

        Iterator getStartIterator()
        {
            return Iterator(this, 0);
        }

        ConstIterator getStartIterator() const
        {
            return ConstIterator(this, 0);
        }

        Iterator getEndIterator()
        {
            return Iterator(this, count);
        }

        ConstIterator getEndIterator() const
        {
            return ConstIterator(this, count);
        }

#pragma endregion

#pragma region Modifiers

        /// Appends at the back. Asserts when full.
        void append(T const& value)
        {
            ARK_ASSERT(!isFull());
            SizeType index = physicalIndex(count);
            new (&data[index]) T(value);
            ++count;
        }

        /// Appends at the back. Asserts when full.
        void append(T&& value)
        {
            ARK_ASSERT(!isFull());
            SizeType index = physicalIndex(count);
            new (&data[index]) T(Ark::move(value));
            ++count;
        }

        template <typename... Args>
        Reference appendMake(Args&&... args)
        {
            ARK_ASSERT(!isFull());
            SizeType index = physicalIndex(count);
            new (&data[index]) T(Ark::forward<Args>(args)...);
            ++count;
            return data[index];
        }

        /// Tries to append; returns false when full.
        bool tryAppend(T const& value)
        {
            if (isFull())
            {
                return false;
            }
            append(value);
            return true;
        }

        /// Tries to append; returns false when full.
        bool tryAppend(T&& value)
        {
            if (isFull())
            {
                return false;
            }
            append(Ark::move(value));
            return true;
        }

        /// Prepends at the front. Asserts when full.
        void prepend(T const& value)
        {
            ARK_ASSERT(!isFull());
            head = (head == 0) ? capacity - 1 : head - 1;
            new (&data[head]) T(value);
            ++count;
        }

        /// Prepends at the front. Asserts when full.
        void prepend(T&& value)
        {
            ARK_ASSERT(!isFull());
            head = (head == 0) ? capacity - 1 : head - 1;
            new (&data[head]) T(Ark::move(value));
            ++count;
        }

        template <typename... Args>
        Reference prependMake(Args&&... args)
        {
            ARK_ASSERT(!isFull());
            head = (head == 0) ? capacity - 1 : head - 1;
            new (&data[head]) T(Ark::forward<Args>(args)...);
            ++count;
            return data[head];
        }

        bool tryPrepend(T const& value)
        {
            if (isFull())
            {
                return false;
            }
            prepend(value);
            return true;
        }

        bool tryPrepend(T&& value)
        {
            if (isFull())
            {
                return false;
            }
            prepend(Ark::move(value));
            return true;
        }

        /// Removes and returns the front element.
        Option<T> removeFirst()
        {
            if (isEmpty())
            {
                return none;
            }

            T value = Ark::move(data[head]);
            data[head].~T();
            head = (head + 1) % capacity;
            --count;
            return value;
        }

        /// Removes and returns the back element.
        Option<T> removeLast()
        {
            if (isEmpty())
            {
                return none;
            }

            SizeType index = physicalIndex(count - 1);
            T value = Ark::move(data[index]);
            data[index].~T();
            --count;
            return value;
        }

        void removeAll()
        {
            while (!isEmpty())
            {
                (void)removeFirst();
            }
        }

        void swap(CircularBuffer& other)
        {
            if (this == &other)
            {
                return;
            }

            if (&allocator != &other.allocator)
            {
                CircularBuffer thisCopy(*this);
                CircularBuffer otherCopy(other);
                *this = Ark::move(otherCopy);
                other = Ark::move(thisCopy);
                return;
            }

            T* tmpData = data;
            SizeType tmpCapacity = capacity;
            SizeType tmpHead = head;
            SizeType tmpCount = count;

            data = other.data;
            capacity = other.capacity;
            head = other.head;
            count = other.count;

            other.data = tmpData;
            other.capacity = tmpCapacity;
            other.head = tmpHead;
            other.count = tmpCount;
        }

#pragma endregion

#pragma region Operators

        Reference operator[](SizeType index)
        {
            ARK_ASSERT(index < count);
            return data[physicalIndex(index)];
        }

        ConstReference operator[](SizeType index) const
        {
            ARK_ASSERT(index < count);
            return data[physicalIndex(index)];
        }

#pragma endregion

#pragma region STL Compatibility API

        using value_type = ValueType;
        using size_type = SizeType;
        using difference_type = DifferenceType;
        using reference = Reference;
        using const_reference = ConstReference;
        using pointer = Pointer;
        using const_pointer = ConstPointer;
        using iterator = Iterator;
        using const_iterator = ConstIterator;

        iterator begin()
        {
            return getStartIterator();
        }

        const_iterator begin() const
        {
            return getStartIterator();
        }

        iterator end()
        {
            return getEndIterator();
        }

        const_iterator end() const
        {
            return getEndIterator();
        }

        size_type size() const
        {
            return getCount();
        }

        bool empty() const
        {
            return isEmpty();
        }

        bool full() const
        {
            return isFull();
        }

        reference front()
        {
            return getFirst();
        }

        const_reference front() const
        {
            return getFirst();
        }

        reference back()
        {
            return getLast();
        }

        const_reference back() const
        {
            return getLast();
        }

        void push_back(T const& value)
        {
            append(value);
        }

        void push_back(T&& value)
        {
            append(Ark::move(value));
        }

        void push_front(T const& value)
        {
            prepend(value);
        }

        void push_front(T&& value)
        {
            prepend(Ark::move(value));
        }

        void pop_front()
        {
            (void)removeFirst();
        }

        void pop_back()
        {
            (void)removeLast();
        }

        void clear()
        {
            removeAll();
        }

#pragma endregion

    private:
        SizeType physicalIndex(SizeType logicalIndex) const
        {
            return (head + logicalIndex) % capacity;
        }

        void reserveCapacity(SizeType newCapacity)
        {
            ARK_ASSERT(count == 0);
            if (newCapacity == 0)
            {
                return;
            }

            if (data != nullptr)
            {
                allocator.deallocate(data);
            }

            data = static_cast<T*>(allocator.allocate(static_cast<uint>(newCapacity * sizeof(T))));
            ARK_ASSERT(data != nullptr);
            capacity = newCapacity;
            head = 0;
        }
    };
}
