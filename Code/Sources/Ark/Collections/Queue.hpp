#pragma once

#include "Ark/Collections/Deque.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections
{
    /// A FIFO queue built on a growable deque.
    /// @tparam T Element type.
    /// @tparam Policy Growth policy forwarded to the underlying deque.
    template <typename T, ArrayPolicy Policy = ArrayPolicyType>
    struct Queue final
    {
    private:
        Deque<T, Policy> storage;

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
        using Iterator = typename Deque<T, Policy>::Iterator;
        using ConstIterator = typename Deque<T, Policy>::ConstIterator;

#pragma endregion

#pragma region Constructors and Assignments

        Queue(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : storage(allocator)
        {
        }

        Queue(InitializerList<T> initializerList, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : storage(initializerList, allocator)
        {
        }

        Queue(Queue const&) = default;
        Queue(Queue&&) = default;
        Queue& operator=(Queue const&) = default;
        Queue& operator=(Queue&&) = default;

        static Queue withCapacity(SizeType minCapacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
        {
            Queue result(allocator);
            result.reserve(minCapacity);
            return result;
        }

#pragma endregion

#pragma region Accessors

        SizeType getCount() const
        {
            return storage.getCount();
        }

        SizeType getCapacity() const
        {
            return storage.getCapacity();
        }

        bool isEmpty() const
        {
            return storage.isEmpty();
        }

        Reference getFirst()
        {
            return storage.getFirst();
        }

        ConstReference getFirst() const
        {
            return storage.getFirst();
        }

        Reference getLast()
        {
            return storage.getLast();
        }

        ConstReference getLast() const
        {
            return storage.getLast();
        }

        Option<Reference> tryGetFirst()
        {
            return storage.tryGetFirst();
        }

        Option<ConstReference> tryGetFirst() const
        {
            return storage.tryGetFirst();
        }

        Option<Reference> tryGetLast()
        {
            return storage.tryGetLast();
        }

        Option<ConstReference> tryGetLast() const
        {
            return storage.tryGetLast();
        }

        AllocatorType getAllocator() const
        {
            return storage.getAllocator();
        }

#pragma endregion

#pragma region Iterators

        Iterator getStartIterator()
        {
            return storage.getStartIterator();
        }

        ConstIterator getStartIterator() const
        {
            return storage.getStartIterator();
        }

        Iterator getEndIterator()
        {
            return storage.getEndIterator();
        }

        ConstIterator getEndIterator() const
        {
            return storage.getEndIterator();
        }

#pragma endregion

#pragma region Modifiers

        void reserve(SizeType minCapacity)
        {
            storage.reserve(minCapacity);
        }

        /// Enqueues at the back.
        void enqueue(T const& value)
        {
            storage.append(value);
        }

        /// Enqueues at the back.
        void enqueue(T&& value)
        {
            storage.append(Ark::move(value));
        }

        template <typename... Args>
        Reference enqueueMake(Args&&... args)
        {
            return storage.appendMake(Ark::forward<Args>(args)...);
        }

        /// Dequeues from the front.
        Option<T> dequeue()
        {
            return storage.removeFirst();
        }

        /// Alias for enqueue (Ark collection naming).
        void append(T const& value)
        {
            enqueue(value);
        }

        void append(T&& value)
        {
            enqueue(Ark::move(value));
        }

        Option<T> removeFirst()
        {
            return dequeue();
        }

        void removeAll()
        {
            storage.removeAll();
        }

        void swap(Queue& other)
        {
            storage.swap(other.storage);
        }

#pragma endregion

#pragma region STL Compatibility API

        using value_type = ValueType;
        using size_type = SizeType;
        using reference = Reference;
        using const_reference = ConstReference;
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

        void push(T const& value)
        {
            enqueue(value);
        }

        void push(T&& value)
        {
            enqueue(Ark::move(value));
        }

        void pop()
        {
            (void)dequeue();
        }

        void clear()
        {
            removeAll();
        }

#pragma endregion
    };
}
