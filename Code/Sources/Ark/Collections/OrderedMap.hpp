#pragma once

#include "Ark/Collections/Internal/RedBlackTree.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections
{
    /// Ordered associative container storing unique keys and mapped values.
    /// Iteration order follows key ordering defined by `Compare`.
    template <typename Key, typename T, typename Compare = Traits::Less<Key>>
    struct OrderedMap final
    {
    private:
        using KeyStorageType = Traits::RemoveCVType<Key>;

#pragma region Types

    public:
        using KeyType = Key;
        using MappedType = T;
        using ValueType = Pair<KeyStorageType const, T>;
        using SizeType = usize;
        using DifferenceType = isize;
        using KeyCompareType = Compare;
        using AllocatorType = Memory::Allocator*;
        using Reference = ValueType&;
        using ConstReference = ValueType const&;
        using Pointer = ValueType*;
        using ConstPointer = ValueType const*;

    private:
        struct KeyAccessor final
        {
            KeyType const& operator()(ValueType const& value) const
            {
                return value.first;
            }
        };

        using TreeType = Internal::RedBlackTree<KeyType, ValueType, KeyAccessor, Compare>;
        using Node = typename TreeType::Node;

        TreeType tree;

    public:
        /// Bidirectional iterator over ordered key-value entries.
        struct Iterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = Pointer;
            using reference = Reference;

            Node* node{nullptr};
            TreeType* treeReference{nullptr};

            Iterator() = default;

            Iterator(Node* nodeValue, TreeType* treeValue)
                : node(nodeValue)
                , treeReference(treeValue)
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
                if (treeReference != nullptr)
                {
                    node = treeReference->getNextNode(node);
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
                if (treeReference != nullptr)
                {
                    node = treeReference->getPreviousNode(node);
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
                return (node == other.node) && (treeReference == other.treeReference);
            }

            bool operator!=(Iterator const& other) const
            {
                return !(*this == other);
            }
        };

        /// Const bidirectional iterator over ordered key-value entries.
        struct ConstIterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = ConstPointer;
            using reference = ConstReference;

            Node const* node{nullptr};
            TreeType const* treeReference{nullptr};

            ConstIterator() = default;

            ConstIterator(Node const* nodeValue, TreeType const* treeValue)
                : node(nodeValue)
                , treeReference(treeValue)
            {
            }

            ConstIterator(Iterator const& other)
                : node(other.node)
                , treeReference(other.treeReference)
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
                if (treeReference != nullptr)
                {
                    node = treeReference->getNextNode(const_cast<Node*>(node));
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
                if (treeReference != nullptr)
                {
                    node = treeReference->getPreviousNode(const_cast<Node*>(node));
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
                return (node == other.node) && (treeReference == other.treeReference);
            }

            bool operator!=(ConstIterator const& other) const
            {
                return !(*this == other);
            }
        };

#pragma endregion

#pragma region Constructors and Assignments

        /// Constructs an empty ordered map using the provided allocator.
        OrderedMap(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator)
        {
        }

        /// Constructs an empty ordered map with a custom key comparator.
        explicit OrderedMap(KeyCompareType const& compare, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator, compare)
        {
        }

        template <typename InputIterator, typename = Traits::EnableIfType<!Traits::isIntegral<InputIterator>>>
        OrderedMap(InputIterator first, InputIterator last, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator)
        {
            insert(first, last);
        }

        template <typename InputIterator, typename = Traits::EnableIfType<!Traits::isIntegral<InputIterator>>, typename Dummy = void>
        OrderedMap(InputIterator first, InputIterator last, KeyCompareType const& compare, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator, compare)
        {
            insert(first, last);
        }

        OrderedMap(InitializerList<ValueType> initializer, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator)
        {
            insert(initializer);
        }

        OrderedMap(InitializerList<ValueType> initializer, KeyCompareType const& compare, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator, compare)
        {
            insert(initializer);
        }

        OrderedMap(OrderedMap const& other)
            : tree(*other.tree.getAllocator(), other.getKeyCompare())
        {
            for (ConstIterator iterator = other.begin(); iterator != other.end(); ++iterator)
            {
                insert(*iterator);
            }
        }

        OrderedMap(OrderedMap&& other)
            : tree(*other.tree.getAllocator(), other.getKeyCompare())
        {
            swap(other);
        }

        ~OrderedMap()
        {
            removeAll();
        }

        OrderedMap& operator=(OrderedMap const& other)
        {
            if (this != &other)
            {
                OrderedMap copy(other);
                swap(copy);
            }
            return *this;
        }

        OrderedMap& operator=(OrderedMap&& other)
        {
            if (this != &other)
            {
                removeAll();
                swap(other);
            }
            return *this;
        }

#pragma endregion

#pragma region Accessors and Modifiers

        /// Returns `true` when no entries are stored.
        bool isEmpty() const
        {
            return tree.isEmpty();
        }

        /// Returns the number of stored entries.
        SizeType getCount() const
        {
            return tree.getCount();
        }

        /// Returns the number of stored elements.
        SizeType getCount(KeyType const& key) const
        {
            return contains(key) ? 1 : 0;
        }

        /// Removes all elements.
        void removeAll()
        {
            tree.removeAll();
        }

#pragma endregion

#pragma region Lookup

        /// Checks whether the map contains the specified key.
        bool contains(KeyType const& key) const
        {
            return tree.findNode(key) != nullptr;
        }

        /// Returns at.
        MappedType& getAt(KeyType const& key)
        {
            Node* node = tree.findNode(key);
            ARK_ASSERT(node != nullptr);
            return node->value.second;
        }

        /// Returns at.
        MappedType const& getAt(KeyType const& key) const
        {
            Node* node = tree.findNode(key);
            ARK_ASSERT(node != nullptr);
            return node->value.second;
        }

        /// Attempts to get.
        Option<MappedType&> tryGet(KeyType const& key)
        {
            Node* node = tree.findNode(key);
            if (node == nullptr)
            {
                return none;
            }
            return node->value.second;
        }

        /// Attempts to get.
        Option<MappedType const&> tryGet(KeyType const& key) const
        {
            Node* node = tree.findNode(key);
            if (node == nullptr)
            {
                return none;
            }
            return node->value.second;
        }

        /// Finds an element.
        Iterator find(KeyType const& key)
        {
            return Iterator(tree.findNode(key), &tree);
        }

        /// Finds an element.
        ConstIterator find(KeyType const& key) const
        {
            return ConstIterator(tree.findNode(key), &tree);
        }

        /// Returns the first element not less than the provided key.
        Iterator lowerBound(KeyType const& key)
        {
            return Iterator(tree.findLowerBoundNode(key), &tree);
        }

        /// Returns the first element not less than the provided key.
        ConstIterator lowerBound(KeyType const& key) const
        {
            return ConstIterator(tree.findLowerBoundNode(key), &tree);
        }

        /// Returns the first element greater than the provided key.
        Iterator upperBound(KeyType const& key)
        {
            return Iterator(tree.findUpperBoundNode(key), &tree);
        }

        /// Returns the first element greater than the provided key.
        ConstIterator upperBound(KeyType const& key) const
        {
            return ConstIterator(tree.findUpperBoundNode(key), &tree);
        }

#pragma endregion

#pragma region Insertion and Assignment

        /// Inserts a key-value pair when the key does not already exist.
        Pair<Iterator, bool> insert(ValueType const& value)
        {
            auto result = tree.insertUnique(value);
            return {Iterator(result.first, &tree), result.second};
        }

        /// Inserts an element.
        Pair<Iterator, bool> insert(ValueType&& value)
        {
            auto result = tree.insertUnique(Ark::move(value));
            return {Iterator(result.first, &tree), result.second};
        }

        /// Inserts an element.
        Iterator insert(ConstIterator hint, ValueType const& value)
        {
            (void)hint;
            return insert(value).first;
        }

        /// Inserts an element.
        Iterator insert(ConstIterator hint, ValueType&& value)
        {
            (void)hint;
            return insert(Ark::move(value)).first;
        }

        /// Inserts an element.
        template <typename InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            for (; first != last; ++first)
            {
                insert(*first);
            }
        }

        /// Inserts an element.
        void insert(InitializerList<ValueType> initializer)
        {
            insert(initializer.begin(), initializer.end());
        }

        /// Constructs and inserts an element.
        template <typename... Args>
        Pair<Iterator, bool> emplace(Args&&... arguments)
        {
            ValueType value(Ark::forward<Args>(arguments)...);
            return insert(Ark::move(value));
        }

        /// Attempts to emplace.
        template <typename... Args>
        Pair<Iterator, bool> tryEmplace(KeyType const& key, Args&&... arguments)
        {
            Node* existingNode = tree.findNode(key);
            if (existingNode != nullptr)
            {
                return {Iterator(existingNode, &tree), false};
            }

            ValueType value(key, T(Ark::forward<Args>(arguments)...));
            auto result = tree.insertUnique(Ark::move(value));
            return {Iterator(result.first, &tree), result.second};
        }

        /// Attempts to emplace.
        template <typename... Args>
        Pair<Iterator, bool> tryEmplace(KeyType&& key, Args&&... arguments)
        {
            Node* existingNode = tree.findNode(key);
            if (existingNode != nullptr)
            {
                return {Iterator(existingNode, &tree), false};
            }

            ValueType value(Ark::move(key), T(Ark::forward<Args>(arguments)...));
            auto result = tree.insertUnique(Ark::move(value));
            return {Iterator(result.first, &tree), result.second};
        }

        /// Inserts or assign.
        template <typename M>
        Pair<Iterator, bool> insertOrAssign(KeyType const& key, M&& object)
        {
            Node* existingNode = tree.findNode(key);
            if (existingNode != nullptr)
            {
                existingNode->value.second = Ark::forward<M>(object);
                return {Iterator(existingNode, &tree), false};
            }

            ValueType value(key, Ark::forward<M>(object));
            auto result = tree.insertUnique(Ark::move(value));
            return {Iterator(result.first, &tree), result.second};
        }

        /// Inserts or assign.
        template <typename M>
        Pair<Iterator, bool> insertOrAssign(KeyType&& key, M&& object)
        {
            Node* existingNode = tree.findNode(key);
            if (existingNode != nullptr)
            {
                existingNode->value.second = Ark::forward<M>(object);
                return {Iterator(existingNode, &tree), false};
            }

            ValueType value(Ark::move(key), Ark::forward<M>(object));
            auto result = tree.insertUnique(Ark::move(value));
            return {Iterator(result.first, &tree), result.second};
        }

#pragma endregion

#pragma region Removal

        /// Removes an entry by key.
        SizeType remove(KeyType const& key)
        {
            Node* node = tree.findNode(key);
            if (node == nullptr)
            {
                return 0;
            }

            tree.eraseNode(node);
            return 1;
        }

        /// Removes an element.
        Iterator remove(Iterator position)
        {
            if (position.node == nullptr)
            {
                return end();
            }

            Node* nextNode = tree.getNextNode(position.node);
            tree.eraseNode(position.node);
            return Iterator(nextNode, &tree);
        }

        /// Removes an element.
        Iterator remove(ConstIterator position)
        {
            if (position.node == nullptr)
            {
                return end();
            }

            Node* mutableNode = const_cast<Node*>(position.node);
            Node* nextNode = tree.getNextNode(mutableNode);
            tree.eraseNode(mutableNode);
            return Iterator(nextNode, &tree);
        }

#pragma endregion

#pragma region Range Queries

        /// Returns the matching range for a key.
        Pair<Iterator, Iterator> equalRange(KeyType const& key)
        {
            return {lowerBound(key), upperBound(key)};
        }

        /// Returns the range matching the provided key.
        Pair<ConstIterator, ConstIterator> equalRange(KeyType const& key) const
        {
            return {lowerBound(key), upperBound(key)};
        }

#pragma endregion

#pragma region Properties

        /// Returns the key comparator.
        KeyCompareType getKeyCompare() const
        {
            return tree.getCompare();
        }

        /// Returns the allocator used by this container.
        AllocatorType getAllocator() const
        {
            return tree.getAllocator();
        }

#pragma endregion

#pragma region Iterators

        /// Returns an iterator to the first ordered entry.
        Iterator getStartIterator()
        {
            return Iterator(tree.getFirstNode(), &tree);
        }

        /// Returns an iterator to the first element.
        ConstIterator getStartIterator() const
        {
            return ConstIterator(tree.getFirstNode(), &tree);
        }

        /// Returns an iterator to one-past-the-last element.
        Iterator getEndIterator()
        {
            return Iterator(nullptr, &tree);
        }

        /// Returns an iterator to one-past-the-last element.
        ConstIterator getEndIterator() const
        {
            return ConstIterator(nullptr, &tree);
        }

#pragma endregion

#pragma region Operators

        MappedType& operator[](KeyType const& key)
        {
            auto result = tryEmplace(key);
            return result.first->second;
        }

        MappedType& operator[](KeyType&& key)
        {
            auto result = tryEmplace(Ark::move(key));
            return result.first->second;
        }

        /// Swaps contents with another object.
        void swap(OrderedMap& other)
        {
            if (getAllocator() != other.getAllocator())
            {
                OrderedMap thisCopy(*this);
                OrderedMap otherCopy(other);

                removeAll();
                other.removeAll();

                for (ConstIterator iterator = otherCopy.begin(); iterator != otherCopy.end(); ++iterator)
                {
                    insert(*iterator);
                }

                for (ConstIterator iterator = thisCopy.begin(); iterator != thisCopy.end(); ++iterator)
                {
                    other.insert(*iterator);
                }
                return;
            }

            tree.swapStorage(other.tree);
        }

#pragma endregion

#pragma region STL Compatibility API

        using key_type = KeyType;
        using mapped_type = MappedType;
        using value_type = ValueType;
        using size_type = SizeType;
        using difference_type = DifferenceType;
        using key_compare = KeyCompareType;
        using allocator_type = AllocatorType;
        using reference = Reference;
        using const_reference = ConstReference;
        using pointer = Pointer;
        using const_pointer = ConstPointer;
        using iterator = Iterator;
        using const_iterator = ConstIterator;

        size_type size() const
        {
            return getCount();
        }

        bool empty() const
        {
            return isEmpty();
        }

        size_type count(KeyType const& key) const
        {
            return getCount(key);
        }

        void clear()
        {
            removeAll();
        }

        MappedType& at(KeyType const& key)
        {
            return getAt(key);
        }

        MappedType const& at(KeyType const& key) const
        {
            return getAt(key);
        }

        size_type erase(KeyType const& key)
        {
            return remove(key);
        }

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

        iterator erase(iterator position)
        {
            return remove(position);
        }

        iterator erase(const_iterator position)
        {
            return remove(position);
        }

        iterator lower_bound(KeyType const& key)
        {
            return lowerBound(key);
        }

        const_iterator lower_bound(KeyType const& key) const
        {
            return lowerBound(key);
        }

        iterator upper_bound(KeyType const& key)
        {
            return upperBound(key);
        }

        const_iterator upper_bound(KeyType const& key) const
        {
            return upperBound(key);
        }

        Pair<iterator, iterator> equal_range(KeyType const& key)
        {
            return equalRange(key);
        }

        Pair<const_iterator, const_iterator> equal_range(KeyType const& key) const
        {
            return equalRange(key);
        }

        key_compare key_comp() const
        {
            return getKeyCompare();
        }

#pragma endregion
    };

    /// Swaps contents with another object.
    template <typename Key, typename T, typename Compare>
    inline void swap(OrderedMap<Key, T, Compare>& left, OrderedMap<Key, T, Compare>& right)
    {
        left.swap(right);
    }
}
