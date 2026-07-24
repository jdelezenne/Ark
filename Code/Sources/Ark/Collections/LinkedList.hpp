#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections
{
    /// A doubly-linked list with stable node addresses and Ark naming conventions.
    /// @tparam T The type of elements stored in the list.
    template <typename T>
    struct LinkedList final
    {
    private:
        struct Node
        {
            T value;
            Node* prev{nullptr};
            Node* next{nullptr};

            template <typename... Args>
            explicit Node(Args&&... args)
                : value(Ark::forward<Args>(args)...)
            {
            }
        };

        Memory::Allocator& allocator{Memory::getDefaultAllocator()};
        Node* head{nullptr};
        Node* tail{nullptr};
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

            Node* node{nullptr};

            Iterator() = default;
            explicit Iterator(Node* n)
                : node(n)
            {
            }

            reference operator*() const
            {
                return node->value;
            }

            pointer operator->() const
            {
                return &node->value;
            }

            Iterator& operator++()
            {
                if (node != nullptr)
                {
                    node = node->next;
                }
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator copy = *this;
                ++(*this);
                return copy;
            }

            Iterator& operator--()
            {
                if (node != nullptr)
                {
                    node = node->prev;
                }
                return *this;
            }

            Iterator operator--(int)
            {
                Iterator copy = *this;
                --(*this);
                return copy;
            }

            bool operator==(Iterator const& other) const
            {
                return node == other.node;
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

            Node const* node{nullptr};

            ConstIterator() = default;
            explicit ConstIterator(Node const* n)
                : node(n)
            {
            }

            ConstIterator(Iterator const& other)
                : node(other.node)
            {
            }

            reference operator*() const
            {
                return node->value;
            }

            pointer operator->() const
            {
                return &node->value;
            }

            ConstIterator& operator++()
            {
                if (node != nullptr)
                {
                    node = node->next;
                }
                return *this;
            }

            ConstIterator operator++(int)
            {
                ConstIterator copy = *this;
                ++(*this);
                return copy;
            }

            ConstIterator& operator--()
            {
                if (node != nullptr)
                {
                    node = node->prev;
                }
                return *this;
            }

            ConstIterator operator--(int)
            {
                ConstIterator copy = *this;
                --(*this);
                return copy;
            }

            bool operator==(ConstIterator const& other) const
            {
                return node == other.node;
            }

            bool operator!=(ConstIterator const& other) const
            {
                return !(*this == other);
            }
        };

#pragma endregion

#pragma region Constructors and Assignments

        LinkedList(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
        }

        LinkedList(InitializerList<T> initializerList, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
        {
            for (T const& value : initializerList)
            {
                append(value);
            }
        }

        LinkedList(LinkedList const& other)
            : allocator{other.allocator}
        {
            for (ConstIterator it = other.getStartIterator(); it != other.getEndIterator(); ++it)
            {
                append(*it);
            }
        }

        LinkedList(LinkedList&& other)
            : allocator{other.allocator}
            , head{other.head}
            , tail{other.tail}
            , count{other.count}
        {
            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }

        ~LinkedList()
        {
            removeAll();
        }

        LinkedList& operator=(LinkedList const& other)
        {
            if (this != &other)
            {
                removeAll();
                for (ConstIterator it = other.getStartIterator(); it != other.getEndIterator(); ++it)
                {
                    append(*it);
                }
            }
            return *this;
        }

        LinkedList& operator=(LinkedList&& other)
        {
            if (this != &other)
            {
                removeAll();
                head = other.head;
                tail = other.tail;
                count = other.count;
                other.head = nullptr;
                other.tail = nullptr;
                other.count = 0;
            }
            return *this;
        }

#pragma endregion

#pragma region Accessors

        SizeType getCount() const
        {
            return count;
        }

        bool isEmpty() const
        {
            return count == 0;
        }

        Reference getFirst()
        {
            ARK_ASSERT(!isEmpty());
            return head->value;
        }

        ConstReference getFirst() const
        {
            ARK_ASSERT(!isEmpty());
            return head->value;
        }

        Reference getLast()
        {
            ARK_ASSERT(!isEmpty());
            return tail->value;
        }

        ConstReference getLast() const
        {
            ARK_ASSERT(!isEmpty());
            return tail->value;
        }

        Option<Reference> tryGetFirst()
        {
            if (isEmpty())
            {
                return none;
            }
            return head->value;
        }

        Option<ConstReference> tryGetFirst() const
        {
            if (isEmpty())
            {
                return none;
            }
            return head->value;
        }

        Option<Reference> tryGetLast()
        {
            if (isEmpty())
            {
                return none;
            }
            return tail->value;
        }

        Option<ConstReference> tryGetLast() const
        {
            if (isEmpty())
            {
                return none;
            }
            return tail->value;
        }

        AllocatorType getAllocator() const
        {
            return &allocator;
        }

#pragma endregion

#pragma region Iterators

        Iterator getStartIterator()
        {
            return Iterator(head);
        }

        ConstIterator getStartIterator() const
        {
            return ConstIterator(head);
        }

        Iterator getEndIterator()
        {
            return Iterator(nullptr);
        }

        ConstIterator getEndIterator() const
        {
            return ConstIterator(nullptr);
        }

#pragma endregion

#pragma region Modifiers

        /// Appends a value at the end of the list.
        void append(T const& value)
        {
            Node* node = allocator.make<Node>(value);
            linkBack(node);
        }

        /// Appends a value at the end of the list.
        void append(T&& value)
        {
            Node* node = allocator.make<Node>(Ark::move(value));
            linkBack(node);
        }

        /// Constructs and appends an element at the end of the list.
        template <typename... Args>
        Reference appendMake(Args&&... args)
        {
            Node* node = allocator.make<Node>(Ark::forward<Args>(args)...);
            linkBack(node);
            return node->value;
        }

        /// Prepends a value at the front of the list.
        void prepend(T const& value)
        {
            Node* node = allocator.make<Node>(value);
            linkFront(node);
        }

        /// Prepends a value at the front of the list.
        void prepend(T&& value)
        {
            Node* node = allocator.make<Node>(Ark::move(value));
            linkFront(node);
        }

        /// Constructs and prepends an element at the front of the list.
        template <typename... Args>
        Reference prependMake(Args&&... args)
        {
            Node* node = allocator.make<Node>(Ark::forward<Args>(args)...);
            linkFront(node);
            return node->value;
        }

        /// Removes and returns the first element, or none if empty.
        Option<T> removeFirst()
        {
            if (isEmpty())
            {
                return none;
            }

            Node* node = head;
            T value = Ark::move(node->value);
            unlink(node);
            allocator.destroy(node);
            return value;
        }

        /// Removes and returns the last element, or none if empty.
        Option<T> removeLast()
        {
            if (isEmpty())
            {
                return none;
            }

            Node* node = tail;
            T value = Ark::move(node->value);
            unlink(node);
            allocator.destroy(node);
            return value;
        }

        /// Removes the element at the given iterator and returns an iterator to the next element.
        Iterator remove(Iterator position)
        {
            ARK_ASSERT(position.node != nullptr);
            Node* next = position.node->next;
            unlink(position.node);
            allocator.destroy(position.node);
            return Iterator(next);
        }

        /// Appends all nodes from another list, leaving it empty.
        void appendList(LinkedList& other)
        {
            if (other.isEmpty())
            {
                return;
            }

            if (isEmpty())
            {
                head = other.head;
                tail = other.tail;
                count = other.count;
            }
            else
            {
                tail->next = other.head;
                other.head->prev = tail;
                tail = other.tail;
                count += other.count;
            }

            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }

        /// Splits the list at the given iterator. Elements from position onward move to the returned list.
        LinkedList splitAt(Iterator position)
        {
            LinkedList result(allocator);

            if (position.node == nullptr)
            {
                return result;
            }

            Node* splitNode = position.node;
            result.head = splitNode;
            result.tail = tail;

            if (splitNode->prev != nullptr)
            {
                splitNode->prev->next = nullptr;
                tail = splitNode->prev;
            }
            else
            {
                head = nullptr;
                tail = nullptr;
            }

            splitNode->prev = nullptr;

            SizeType remaining = 0;
            for (Node* node = head; node != nullptr; node = node->next)
            {
                ++remaining;
            }
            result.count = count - remaining;
            count = remaining;

            return result;
        }

        /// Splits the list after the first `index` elements.
        LinkedList splitAt(SizeType index)
        {
            if (index >= count)
            {
                return LinkedList(allocator);
            }

            Iterator it = getStartIterator();
            for (SizeType i = 0; i < index; ++i)
            {
                ++it;
            }
            return splitAt(it);
        }

        void removeAll()
        {
            Node* node = head;
            while (node != nullptr)
            {
                Node* next = node->next;
                allocator.destroy(node);
                node = next;
            }
            head = nullptr;
            tail = nullptr;
            count = 0;
        }

        void swap(LinkedList& other)
        {
            if (this == &other)
            {
                return;
            }

            if (&allocator == &other.allocator)
            {
                Node* tmpHead = head;
                Node* tmpTail = tail;
                SizeType tmpCount = count;

                head = other.head;
                tail = other.tail;
                count = other.count;

                other.head = tmpHead;
                other.tail = tmpTail;
                other.count = tmpCount;
            }
            else
            {
                LinkedList thisCopy(*this);
                LinkedList otherCopy(other);
                *this = Ark::move(otherCopy);
                other = Ark::move(thisCopy);
            }
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

        void pop_back()
        {
            (void)removeLast();
        }

        void pop_front()
        {
            (void)removeFirst();
        }

        void clear()
        {
            removeAll();
        }

#pragma endregion

    private:
        void linkBack(Node* node)
        {
            node->prev = tail;
            node->next = nullptr;
            if (tail != nullptr)
            {
                tail->next = node;
            }
            else
            {
                head = node;
            }
            tail = node;
            ++count;
        }

        void linkFront(Node* node)
        {
            node->next = head;
            node->prev = nullptr;
            if (head != nullptr)
            {
                head->prev = node;
            }
            else
            {
                tail = node;
            }
            head = node;
            ++count;
        }

        void unlink(Node* node)
        {
            if (node->prev != nullptr)
            {
                node->prev->next = node->next;
            }
            else
            {
                head = node->next;
            }

            if (node->next != nullptr)
            {
                node->next->prev = node->prev;
            }
            else
            {
                tail = node->prev;
            }

            node->prev = nullptr;
            node->next = nullptr;
            --count;
        }
    };
}
