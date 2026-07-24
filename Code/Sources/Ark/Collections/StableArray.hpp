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
    /// A growable array with stable indices and O(1) removal via vacant slots (like Rust's stable_vec).
    /// @details Indices remain valid until the element at that index is removed. Removal does not
    /// compact other elements. Memory usage grows with `getNextAppendIndex()`, not live element count.
    /// @tparam T Element type.
    /// @tparam Policy Growth policy for the slot buffer.
    template <typename T, ArrayPolicy Policy = ArrayPolicyType>
    struct StableArray final
    {
    private:
        struct Slot
        {
            alignas(T) unsigned char storage[sizeof(T)];
            bool occupied{false};

            T* asPointer()
            {
                return reinterpret_cast<T*>(storage);
            }

            T const* asPointer() const
            {
                return reinterpret_cast<T const*>(storage);
            }
        };

        Memory::Allocator& allocator{Memory::getDefaultAllocator()};
        Slot* slots{nullptr};
        usize capacity{0};
        usize filledCount{0};
        usize nextAppendIndex{0};

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

            StableArray* owner{nullptr};
            SizeType index{0};

            Iterator() = default;
            Iterator(StableArray* o, SizeType i)
                : owner(o)
                , index(i)
            {
            }

            reference operator*() const
            {
                return owner->get(index);
            }

            pointer operator->() const
            {
                return &owner->get(index);
            }

            SizeType getIndex() const
            {
                return index;
            }

            Iterator& operator++()
            {
                if (owner != nullptr)
                {
                    index = owner->findNextFilled(index + 1);
                }
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
                return owner == other.owner && index == other.index;
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

            StableArray const* owner{nullptr};
            SizeType index{0};

            ConstIterator() = default;
            ConstIterator(StableArray const* o, SizeType i)
                : owner(o)
                , index(i)
            {
            }

            ConstIterator(Iterator const& other)
                : owner(other.owner)
                , index(other.index)
            {
            }

            reference operator*() const
            {
                return owner->get(index);
            }

            pointer operator->() const
            {
                return &owner->get(index);
            }

            SizeType getIndex() const
            {
                return index;
            }

            ConstIterator& operator++()
            {
                if (owner != nullptr)
                {
                    index = owner->findNextFilled(index + 1);
                }
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
                return owner == other.owner && index == other.index;
            }

            bool operator!=(ConstIterator const& other) const
            {
                return !(*this == other);
            }
        };

#pragma endregion

#pragma region Constructors and Assignments

        StableArray(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
        }

        StableArray(InitializerList<T> initializerList, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
            reserve(initializerList.size());
            for (T const& value : initializerList)
            {
                append(value);
            }
        }

        StableArray(StableArray const& other)
            : allocator{other.allocator}
        {
            reserve(other.capacity);
            for (SizeType i = 0; i < other.nextAppendIndex; ++i)
            {
                if (other.slots[i].occupied)
                {
                    insert(i, *other.slots[i].asPointer());
                }
            }
        }

        StableArray(StableArray&& other)
            : allocator{other.allocator}
            , slots{other.slots}
            , capacity{other.capacity}
            , filledCount{other.filledCount}
            , nextAppendIndex{other.nextAppendIndex}
        {
            other.slots = nullptr;
            other.capacity = 0;
            other.filledCount = 0;
            other.nextAppendIndex = 0;
        }

        ~StableArray()
        {
            removeAll();
            if (slots != nullptr)
            {
                allocator.deallocate(slots);
                slots = nullptr;
            }
        }

        StableArray& operator=(StableArray const& other)
        {
            if (this != &other)
            {
                removeAll();
                reserve(other.capacity);
                for (SizeType i = 0; i < other.nextAppendIndex; ++i)
                {
                    if (other.slots[i].occupied)
                    {
                        insert(i, *other.slots[i].asPointer());
                    }
                }
            }
            return *this;
        }

        StableArray& operator=(StableArray&& other)
        {
            if (this != &other)
            {
                removeAll();
                if (slots != nullptr)
                {
                    allocator.deallocate(slots);
                }

                slots = other.slots;
                capacity = other.capacity;
                filledCount = other.filledCount;
                nextAppendIndex = other.nextAppendIndex;

                other.slots = nullptr;
                other.capacity = 0;
                other.filledCount = 0;
                other.nextAppendIndex = 0;
            }
            return *this;
        }

        /// Constructs with the requested slot capacity.
        static StableArray withCapacity(SizeType slotCapacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
        {
            StableArray result(allocator);
            result.reserve(slotCapacity);
            return result;
        }

#pragma endregion

#pragma region Accessors

        /// Returns the number of filled slots.
        SizeType getCount() const
        {
            return filledCount;
        }

        /// Returns the number of allocated slots (filled and vacant).
        SizeType getCapacity() const
        {
            return capacity;
        }

        /// Returns the index that the next `append` would use.
        SizeType getNextAppendIndex() const
        {
            return nextAppendIndex;
        }

        bool isEmpty() const
        {
            return filledCount == 0;
        }

        /// Returns whether the slot at `index` is occupied.
        bool has(SizeType index) const
        {
            return index < capacity && slots[index].occupied;
        }

        bool containsIndex(SizeType index) const
        {
            return has(index);
        }

        Reference get(SizeType index)
        {
            ARK_ASSERT(has(index));
            return *slots[index].asPointer();
        }

        ConstReference get(SizeType index) const
        {
            ARK_ASSERT(has(index));
            return *slots[index].asPointer();
        }

        Option<Reference> tryGet(SizeType index)
        {
            if (!has(index))
            {
                return none;
            }
            return *slots[index].asPointer();
        }

        Option<ConstReference> tryGet(SizeType index) const
        {
            if (!has(index))
            {
                return none;
            }
            return *slots[index].asPointer();
        }

        AllocatorType getAllocator() const
        {
            return &allocator;
        }

#pragma endregion

#pragma region Iterators

        Iterator getStartIterator()
        {
            return Iterator(this, findNextFilled(0));
        }

        ConstIterator getStartIterator() const
        {
            return ConstIterator(this, findNextFilled(0));
        }

        Iterator getEndIterator()
        {
            return Iterator(this, nextAppendIndex);
        }

        ConstIterator getEndIterator() const
        {
            return ConstIterator(this, nextAppendIndex);
        }

#pragma endregion

#pragma region Modifiers

        /// Inserts at `getNextAppendIndex()` and returns the stable index.
        SizeType append(T const& value)
        {
            SizeType index = nextAppendIndex;
            ensureCapacityForIndex(index);
            constructAt(index, value);
            if (index + 1 > nextAppendIndex)
            {
                nextAppendIndex = index + 1;
            }
            return index;
        }

        /// Inserts at `getNextAppendIndex()` and returns the stable index.
        SizeType append(T&& value)
        {
            SizeType index = nextAppendIndex;
            ensureCapacityForIndex(index);
            constructAt(index, Ark::move(value));
            if (index + 1 > nextAppendIndex)
            {
                nextAppendIndex = index + 1;
            }
            return index;
        }

        /// Constructs at `getNextAppendIndex()` and returns the stable index.
        template <typename... Args>
        SizeType appendMake(Args&&... args)
        {
            SizeType index = nextAppendIndex;
            ensureCapacityForIndex(index);
            constructAt(index, Ark::forward<Args>(args)...);
            if (index + 1 > nextAppendIndex)
            {
                nextAppendIndex = index + 1;
            }
            return index;
        }

        /// Inserts at `index`. Returns the previous value if the slot was occupied.
        /// @details `index` must be less than capacity, or equal to `getNextAppendIndex()` to extend.
        Option<T> insert(SizeType index, T const& value)
        {
            return insertImpl(index, value);
        }

        /// Inserts at `index`. Returns the previous value if the slot was occupied.
        Option<T> insert(SizeType index, T&& value)
        {
            return insertImpl(index, Ark::move(value));
        }

        /// Removes and returns the element at `index`, or none if vacant/out of range.
        Option<T> remove(SizeType index)
        {
            if (!has(index))
            {
                return none;
            }

            T value = Ark::move(*slots[index].asPointer());
            destroyAt(index);
            return value;
        }

        void reserve(SizeType newCapacity)
        {
            if (newCapacity > capacity)
            {
                reallocate(newCapacity);
            }
        }

        /// Ensures slots exist through `index` (inclusive), creating vacant slots as needed.
        void reserveFor(SizeType index)
        {
            ensureCapacityForIndex(index);
            if (index + 1 > nextAppendIndex)
            {
                nextAppendIndex = index + 1;
            }
        }

        void removeAll()
        {
            for (SizeType i = 0; i < nextAppendIndex; ++i)
            {
                if (slots != nullptr && slots[i].occupied)
                {
                    destroyAt(i);
                }
            }
            filledCount = 0;
            nextAppendIndex = 0;
        }

        void swap(StableArray& other)
        {
            if (this == &other)
            {
                return;
            }

            if (&allocator != &other.allocator)
            {
                StableArray thisCopy(*this);
                StableArray otherCopy(other);
                *this = Ark::move(otherCopy);
                other = Ark::move(thisCopy);
                return;
            }

            Slot* tmpSlots = slots;
            SizeType tmpCapacity = capacity;
            SizeType tmpFilled = filledCount;
            SizeType tmpNext = nextAppendIndex;

            slots = other.slots;
            capacity = other.capacity;
            filledCount = other.filledCount;
            nextAppendIndex = other.nextAppendIndex;

            other.slots = tmpSlots;
            other.capacity = tmpCapacity;
            other.filledCount = tmpFilled;
            other.nextAppendIndex = tmpNext;
        }

#pragma endregion

#pragma region Operators

        Reference operator[](SizeType index)
        {
            return get(index);
        }

        ConstReference operator[](SizeType index) const
        {
            return get(index);
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

        const_iterator cbegin() const
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

        const_iterator cend() const
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

        void clear()
        {
            removeAll();
        }

        void push_back(T const& value)
        {
            (void)append(value);
        }

        void push_back(T&& value)
        {
            (void)append(Ark::move(value));
        }

#pragma endregion

    private:
        SizeType findNextFilled(SizeType start) const
        {
            SizeType i = start;
            while (i < nextAppendIndex)
            {
                if (slots[i].occupied)
                {
                    return i;
                }
                ++i;
            }
            return nextAppendIndex;
        }

        void ensureCapacityForIndex(SizeType index)
        {
            if (index < capacity)
            {
                return;
            }

            SizeType newCapacity = capacity == 0 ? 1 : capacity;
            while (newCapacity <= index)
            {
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
                    newCapacity = index + 1;
                }
            }

            newCapacity = Ark::max(newCapacity, Policy::MinimumCapacity);
            if constexpr (Policy::Granularity > 0)
            {
                newCapacity = (newCapacity + Policy::Granularity - 1) / Policy::Granularity * Policy::Granularity;
            }

            reallocate(newCapacity);
        }

        void reallocate(SizeType newCapacity)
        {
            Slot* newSlots = static_cast<Slot*>(allocator.allocate(static_cast<uint>(newCapacity * sizeof(Slot))));
            ARK_ASSERT(newSlots != nullptr);

            for (SizeType i = 0; i < newCapacity; ++i)
            {
                new (&newSlots[i]) Slot();
            }

            for (SizeType i = 0; i < nextAppendIndex; ++i)
            {
                if (slots[i].occupied)
                {
                    new (newSlots[i].asPointer()) T(Ark::move(*slots[i].asPointer()));
                    newSlots[i].occupied = true;
                    slots[i].asPointer()->~T();
                    slots[i].occupied = false;
                }
            }

            if (slots != nullptr)
            {
                for (SizeType i = 0; i < capacity; ++i)
                {
                    slots[i].~Slot();
                }
                allocator.deallocate(slots);
            }

            slots = newSlots;
            capacity = newCapacity;
        }

        template <typename... Args>
        void constructAt(SizeType index, Args&&... args)
        {
            ARK_ASSERT(index < capacity);
            ARK_ASSERT(!slots[index].occupied);
            new (slots[index].asPointer()) T(Ark::forward<Args>(args)...);
            slots[index].occupied = true;
            ++filledCount;
        }

        void destroyAt(SizeType index)
        {
            ARK_ASSERT(slots[index].occupied);
            slots[index].asPointer()->~T();
            slots[index].occupied = false;
            --filledCount;
        }

        template <typename U>
        Option<T> insertImpl(SizeType index, U&& value)
        {
            if (index > nextAppendIndex && index >= capacity)
            {
                // Allow insert into reserved vacant slots or at/after nextAppendIndex by growing.
            }

            ensureCapacityForIndex(index);

            if (index >= nextAppendIndex)
            {
                // Fill gaps as vacant (already vacant from default Slot construction).
                nextAppendIndex = index + 1;
            }

            if (slots[index].occupied)
            {
                T old = Ark::move(*slots[index].asPointer());
                *slots[index].asPointer() = Ark::forward<U>(value);
                return old;
            }

            constructAt(index, Ark::forward<U>(value));
            return none;
        }
    };
}
