#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections
{
    /// A LIFO stack built on a growable array.
    /// @tparam T Element type.
    /// @tparam Policy Growth policy forwarded to the underlying array.
    template <typename T, ArrayPolicy Policy = ArrayPolicyType>
    struct Stack final
    {
    private:
        Array<T, Policy> storage;

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
        using Iterator = typename Array<T, Policy>::Iterator;
        using ConstIterator = typename Array<T, Policy>::ConstIterator;

#pragma endregion

#pragma region Constructors and Assignments

        Stack(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : storage(allocator)
        {
        }

        Stack(InitializerList<T> initializerList, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : storage(allocator)
        {
            for (T const& value : initializerList)
            {
                push(value);
            }
        }

        Stack(Stack const&) = default;
        Stack(Stack&&) = default;
        Stack& operator=(Stack const&) = default;
        Stack& operator=(Stack&&) = default;

        static Stack withCapacity(SizeType minCapacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
        {
            Stack result(allocator);
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

        /// Returns a reference to the top element.
        Reference getTop()
        {
            return storage.getLast();
        }

        ConstReference getTop() const
        {
            return storage.getLast();
        }

        Option<Reference> tryGetTop()
        {
            if (storage.isEmpty())
            {
                return none;
            }
            return storage.getLast();
        }

        Option<ConstReference> tryGetTop() const
        {
            if (storage.isEmpty())
            {
                return none;
            }
            return storage.getLast();
        }

#pragma endregion

#pragma region Iterators


        /// Bottom-to-top iteration.
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

        void push(T const& value)
        {
            storage.append(value);
        }

        void push(T&& value)
        {
            storage.append(Ark::move(value));
        }

        template <typename... Args>
        Reference pushMake(Args&&... args)
        {
            return storage.appendMake(Ark::forward<Args>(args)...);
        }

        Option<T> pop()
        {
            if (storage.isEmpty())
            {
                return none;
            }

            T value = Ark::move(storage.getLast());
            storage.removeLast();
            return value;
        }

        void removeAll()
        {
            storage.removeAll();
        }

        void swap(Stack& other)
        {
            // Array has no swap; exchange via move when same allocator path is needed.
            Array<T, Policy> temporary = Ark::move(storage);
            storage = Ark::move(other.storage);
            other.storage = Ark::move(temporary);
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

        reference top()
        {
            return getTop();
        }

        const_reference top() const
        {
            return getTop();
        }

        void clear()
        {
            removeAll();
        }

#pragma endregion
    };
}
