#pragma once

#include "Ark/Collections/ArrayPolicy.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

#include <new>

namespace Ark::Collections
{
    /// A growable double-ended queue with O(1) amortized push/pop at both ends.
    /// @tparam T Element type.
    /// @tparam Policy Growth policy for the ring storage.
    template <typename T, ArrayPolicy Policy = ArrayPolicyType>
    struct Deque final
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
        using PolicyType = Policy;
        using AllocatorType = Memory::Allocator*;

        struct Iterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = Pointer;
            using reference = Reference;

            Deque* owner{nullptr};
            SizeType offset{0};

            Iterator() = default;
            Iterator(Deque* o, SizeType off)
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

            Deque const* owner{nullptr};
            SizeType offset{0};

            ConstIterator() = default;
            ConstIterator(Deque const* o, SizeType off)
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

        Deque(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
        }

        Deque(InitializerList<T> initializerList, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
            for (T const& value : initializerList)
            {
                append(value);
            }
        }

        Deque(Deque const& other)
            : allocator{other.allocator}
        {
            reserve(other.count);
            for (SizeType i = 0; i < other.count; ++i)
            {
                append(other[i]);
            }
        }

        Deque(Deque&& other)
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

        ~Deque()
        {
            removeAll();
            if (data != nullptr)
            {
                allocator.deallocate(data);
            }
        }

        Deque& operator=(Deque const& other)
        {
            if (this != &other)
            {
                removeAll();
                reserve(other.count);
                for (SizeType i = 0; i < other.count; ++i)
                {
                    append(other[i]);
                }
            }
            return *this;
        }

        Deque& operator=(Deque&& other)
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

        static Deque withCapacity(SizeType minCapacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
        {
            Deque result(allocator);
            result.reserve(minCapacity);
            return result;
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

        void reserve(SizeType minCapacity)
        {
            if (minCapacity > capacity)
            {
                reallocate(minCapacity);
            }
        }

        void append(T const& value)
        {
            ensureCapacityForOne();
            SizeType index = physicalIndex(count);
            new (&data[index]) T(value);
            ++count;
        }

        void append(T&& value)
        {
            ensureCapacityForOne();
            SizeType index = physicalIndex(count);
            new (&data[index]) T(Ark::move(value));
            ++count;
        }

        template <typename... Args>
        Reference appendMake(Args&&... args)
        {
            ensureCapacityForOne();
            SizeType index = physicalIndex(count);
            new (&data[index]) T(Ark::forward<Args>(args)...);
            ++count;
            return data[index];
        }

        void prepend(T const& value)
        {
            ensureCapacityForOne();
            head = (capacity == 0) ? 0 : ((head == 0) ? capacity - 1 : head - 1);
            new (&data[head]) T(value);
            ++count;
        }

        void prepend(T&& value)
        {
            ensureCapacityForOne();
            head = (capacity == 0) ? 0 : ((head == 0) ? capacity - 1 : head - 1);
            new (&data[head]) T(Ark::move(value));
            ++count;
        }

        template <typename... Args>
        Reference prependMake(Args&&... args)
        {
            ensureCapacityForOne();
            head = (capacity == 0) ? 0 : ((head == 0) ? capacity - 1 : head - 1);
            new (&data[head]) T(Ark::forward<Args>(args)...);
            ++count;
            return data[head];
        }

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

        void swap(Deque& other)
        {
            if (this == &other)
            {
                return;
            }

            if (&allocator != &other.allocator)
            {
                Deque thisCopy(*this);
                Deque otherCopy(other);
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

        void ensureCapacityForOne()
        {
            if (count < capacity)
            {
                return;
            }

            SizeType newCapacity = capacity == 0 ? Ark::max(Policy::MinimumCapacity, SizeType(1)) : capacity;

            if constexpr (Policy::GrowStrategy == ResizeStrategy::Ratio)
            {
                SizeType grown = static_cast<SizeType>(static_cast<float32>(newCapacity) * Policy::GrowRatio);
                newCapacity = Ark::max(grown, newCapacity + 1);
            }
            else if constexpr (Policy::GrowStrategy == ResizeStrategy::Count)
            {
                newCapacity = newCapacity + Ark::max(Policy::GrowCount, SizeType(1));
            }
            else
            {
                newCapacity = newCapacity + 1;
            }

            if constexpr (Policy::Granularity > 0)
            {
                newCapacity = (newCapacity + Policy::Granularity - 1) / Policy::Granularity * Policy::Granularity;
            }

            reallocate(newCapacity);
        }

        void reallocate(SizeType newCapacity)
        {
            ARK_ASSERT(newCapacity >= count);

            T* newData = static_cast<T*>(allocator.allocate(static_cast<uint>(newCapacity * sizeof(T))));
            ARK_ASSERT(newData != nullptr);

            for (SizeType i = 0; i < count; ++i)
            {
                new (&newData[i]) T(Ark::move((*this)[i]));
                (*this)[i].~T();
            }

            if (data != nullptr)
            {
                allocator.deallocate(data);
            }

            data = newData;
            capacity = newCapacity;
            head = 0;
        }
    };
}
