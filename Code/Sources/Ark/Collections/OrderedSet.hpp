#pragma once

#include "Ark/Collections/Internal/RedBlackTree.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections
{
    /// Ordered associative container storing unique keys.
    /// Iteration order follows key ordering defined by `Compare`.
    template <typename Key, typename Compare = Traits::Less<Key>>
    struct OrderedSet final
    {
    private:
        using KeyStorageType = Traits::RemoveCVType<Key>;

#pragma region Types

    public:
        using KeyType = Key;
        using ValueType = KeyStorageType;
        using SizeType = usize;
        using DifferenceType = isize;
        using KeyCompareType = Compare;
        using AllocatorType = Memory::Allocator*;
        using Reference = ValueType const&;
        using ConstReference = ValueType const&;
        using Pointer = ValueType const*;
        using ConstPointer = ValueType const*;

    private:
        struct KeyAccessor final
        {
            KeyType const& operator()(ValueType const& value) const
            {
                return value;
            }
        };

        using TreeType = Internal::RedBlackTree<KeyType, ValueType, KeyAccessor, Compare>;
        using Node = typename TreeType::Node;

        TreeType tree;

    public:
        /// Bidirectional iterator over ordered keys.
        struct Iterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = Pointer;
            using reference = Reference;

            Node* node{nullptr};
            TreeType* treeReference{nullptr};

            /// Constructs an end iterator.
            Iterator() = default;

            /// Constructs an iterator from node and tree references.
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

        /// Const bidirectional iterator over ordered keys.
        struct ConstIterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = ConstPointer;
            using reference = ConstReference;

            Node const* node{nullptr};
            TreeType const* treeReference{nullptr};

            /// Constructs an end const iterator.
            ConstIterator() = default;

            /// Constructs a const iterator from node and tree references.
            ConstIterator(Node const* nodeValue, TreeType const* treeValue)
                : node(nodeValue)
                , treeReference(treeValue)
            {
            }

            /// Constructs a const iterator from a mutable iterator.
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

        /// Constructs an empty ordered set using the provided allocator.
        OrderedSet(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator)
        {
        }

        /// Constructs an empty ordered set with a custom key comparator.
        explicit OrderedSet(KeyCompareType const& compare, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator, compare)
        {
        }

        template <typename InputIterator, typename = Traits::EnableIfType<!Traits::isIntegral<InputIterator>>>
        OrderedSet(InputIterator first, InputIterator last, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator)
        {
            insert(first, last);
        }

        template <typename InputIterator, typename = Traits::EnableIfType<!Traits::isIntegral<InputIterator>>, typename Dummy = void>
        OrderedSet(InputIterator first, InputIterator last, KeyCompareType const& compare, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator, compare)
        {
            insert(first, last);
        }

        OrderedSet(InitializerList<ValueType> initializer, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator)
        {
            insert(initializer);
        }

        OrderedSet(InitializerList<ValueType> initializer, KeyCompareType const& compare, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : tree(allocator, compare)
        {
            insert(initializer);
        }

        OrderedSet(OrderedSet const& other)
            : tree(*other.tree.getAllocator(), other.getKeyCompare())
        {
            for (ConstIterator iterator = other.begin(); iterator != other.end(); ++iterator)
            {
                insert(*iterator);
            }
        }

        OrderedSet(OrderedSet&& other)
            : tree(*other.tree.getAllocator(), other.getKeyCompare())
        {
            swap(other);
        }

        ~OrderedSet()
        {
            removeAll();
        }

        OrderedSet& operator=(OrderedSet const& other)
        {
            if (this != &other)
            {
                OrderedSet copy(other);
                swap(copy);
            }
            return *this;
        }

        OrderedSet& operator=(OrderedSet&& other)
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

        /// Returns `true` when no keys are stored.
        bool isEmpty() const
        {
            return tree.isEmpty();
        }

        /// Returns the number of stored elements.
        SizeType getCount() const
        {
            return tree.getCount();
        }

        /// Returns 1 if the key exists, otherwise 0.
        SizeType getCount(KeyType const& key) const
        {
            return contains(key) ? 1 : 0;
        }

        /// Removes all keys from the set.
        void removeAll()
        {
            tree.removeAll();
        }

#pragma endregion

#pragma region Lookup

        /// Checks whether the set contains the specified key.
        bool contains(KeyType const& key) const
        {
            return tree.findNode(key) != nullptr;
        }

        /// Attempts to get.
        Option<ConstReference> tryGet(KeyType const& key) const
        {
            Node* node = tree.findNode(key);
            if (node == nullptr)
            {
                return none;
            }
            return node->value;
        }

        /// Returns an iterator to the matching key, or end.
        Iterator find(KeyType const& key)
        {
            return Iterator(tree.findNode(key), &tree);
        }

        /// Returns a const iterator to the matching key, or end.
        ConstIterator find(KeyType const& key) const
        {
            return ConstIterator(tree.findNode(key), &tree);
        }

        /// Returns the first key not less than the provided key.
        Iterator lowerBound(KeyType const& key)
        {
            return Iterator(tree.findLowerBoundNode(key), &tree);
        }

        /// Returns the first key not less than the provided key.
        ConstIterator lowerBound(KeyType const& key) const
        {
            return ConstIterator(tree.findLowerBoundNode(key), &tree);
        }

        /// Returns the first key greater than the provided key.
        Iterator upperBound(KeyType const& key)
        {
            return Iterator(tree.findUpperBoundNode(key), &tree);
        }

        /// Returns the first key greater than the provided key.
        ConstIterator upperBound(KeyType const& key) const
        {
            return ConstIterator(tree.findUpperBoundNode(key), &tree);
        }

#pragma endregion

#pragma region Insertion and Assignment

        /// Inserts a key when it does not already exist.
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

        /// Inserts with a positional hint.
        Iterator insert(ConstIterator hint, ValueType const& value)
        {
            (void)hint;
            return insert(value).first;
        }

        /// Inserts with a positional hint.
        Iterator insert(ConstIterator hint, ValueType&& value)
        {
            (void)hint;
            return insert(Ark::move(value)).first;
        }

        /// Inserts all keys from the iterator range.
        /// Inserts an element.
        template <typename InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            for (; first != last; ++first)
            {
                insert(*first);
            }
        }

        /// Inserts all keys from the initializer list.
        void insert(InitializerList<ValueType> initializer)
        {
            insert(initializer.begin(), initializer.end());
        }

        /// Constructs and inserts a key in-place.
        /// Constructs and inserts an element.
        template <typename... Args>
        Pair<Iterator, bool> emplace(Args&&... arguments)
        {
            ValueType value(Ark::forward<Args>(arguments)...);
            return insert(Ark::move(value));
        }

#pragma endregion

#pragma region Removal

        /// Removes a key from the set.
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

        /// Removes the key at the given const iterator position.
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

        /// Returns an iterator to the first ordered key.
        Iterator getStartIterator()
        {
            return Iterator(tree.getFirstNode(), &tree);
        }

        /// Returns an iterator to the first element.
        ConstIterator getStartIterator() const
        {
            return ConstIterator(tree.getFirstNode(), &tree);
        }

        /// Returns an iterator to one-past-the-last key.
        Iterator getEndIterator()
        {
            return Iterator(nullptr, &tree);
        }

        /// Returns a const iterator to one-past-the-last key.
        ConstIterator getEndIterator() const
        {
            return ConstIterator(nullptr, &tree);
        }

#pragma endregion

#pragma region Operators

        /// Swaps contents with another ordered set.
        void swap(OrderedSet& other)
        {
            if (getAllocator() != other.getAllocator())
            {
                OrderedSet thisCopy(*this);
                OrderedSet otherCopy(other);

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

    /// Swaps two ordered sets.
    template <typename Key, typename Compare>
    inline void swap(OrderedSet<Key, Compare>& left, OrderedSet<Key, Compare>& right)
    {
        left.swap(right);
    }
}
