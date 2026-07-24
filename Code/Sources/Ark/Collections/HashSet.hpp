#pragma once

#include "Ark/Collections/Algorithms.hpp"
#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Hasher.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections
{
    namespace Internal
    {
        template <typename Key>
        struct HashSetDefaultHash final
        {
            usize operator()(Key const& value) const
            {
                if constexpr (requires { Hasher<Key>{}(value); })
                {
                    return Hasher<Key>{}(value);
                }
                else if constexpr (requires {
                                       value.asPointer();
                                       value.getCount();
                                   })
                {
                    return computeHashArray(value.asPointer(), value.getCount());
                }
                else
                {
                    static_assert(Traits::isSame<Key, void>, "HashSet requires a hash function for this key type.");
                    return 0;
                }
            }
        };
    }

    /// Hash set implementation with stable node addresses and Ark naming conventions.
    template <typename Key, typename Hash = Internal::HashSetDefaultHash<Key>, typename KeyEqual = EqualTo<Key>>
    struct HashSet final
    {
    private:
        using KeyStorageType = Traits::RemoveCVType<Key>;

        struct Node
        {
            KeyStorageType value;
            Node* bucketNext{nullptr};
            Node* orderPrev{nullptr};
            Node* orderNext{nullptr};

            Node(KeyStorageType const& v)
                : value(v)
            {
            }

            Node(KeyStorageType&& v)
                : value(Ark::move(v))
            {
            }

            template <typename K>
            explicit Node(K&& key)
                : value(Ark::forward<K>(key))
            {
            }
        };

        static constexpr usize DefaultBucketCount = 16;

        Memory::Allocator& allocator{Memory::getDefaultAllocator()};
        Array<Node*> buckets;
        Node* orderHead{nullptr};
        Node* orderTail{nullptr};
        usize entryCount{0};
        float maxLoad{1.0f};
        Hash hashFunction{};
        KeyEqual keyEqual{};

    public:
#pragma region Types

    public:
        using KeyType = Key;
        using ValueType = KeyStorageType;
        using SizeType = usize;
        using DifferenceType = isize;
        using HasherType = Hash;
        using KeyEqualType = KeyEqual;
        using AllocatorType = Memory::Allocator*;
        using Reference = ValueType const&;
        using ConstReference = ValueType const&;
        using Pointer = ValueType const*;
        using ConstPointer = ValueType const*;

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
                    node = node->orderNext;
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
                    node = node->orderNext;
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
                return node == other.node;
            }

            bool operator!=(ConstIterator const& other) const
            {
                return !(*this == other);
            }
        };

        struct LocalIterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = Pointer;
            using reference = Reference;

            Node* node{nullptr};

            LocalIterator() = default;
            explicit LocalIterator(Node* n)
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

            LocalIterator& operator++()
            {
                if (node != nullptr)
                {
                    node = node->bucketNext;
                }
                return *this;
            }

            LocalIterator operator++(int)
            {
                LocalIterator copy = *this;
                ++(*this);
                return copy;
            }

            bool operator==(LocalIterator const& other) const
            {
                return node == other.node;
            }

            bool operator!=(LocalIterator const& other) const
            {
                return !(*this == other);
            }
        };

        struct ConstLocalIterator
        {
            using value_type = ValueType;
            using difference_type = DifferenceType;
            using pointer = ConstPointer;
            using reference = ConstReference;

            Node const* node{nullptr};

            ConstLocalIterator() = default;
            explicit ConstLocalIterator(Node const* n)
                : node(n)
            {
            }

            ConstLocalIterator(LocalIterator const& other)
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

            ConstLocalIterator& operator++()
            {
                if (node != nullptr)
                {
                    node = node->bucketNext;
                }
                return *this;
            }

            ConstLocalIterator operator++(int)
            {
                ConstLocalIterator copy = *this;
                ++(*this);
                return copy;
            }

            bool operator==(ConstLocalIterator const& other) const
            {
                return node == other.node;
            }

            bool operator!=(ConstLocalIterator const& other) const
            {
                return !(*this == other);
            }
        };

#pragma endregion

#pragma region Constructors and Assignments

    public:
        HashSet(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(DefaultBucketCount, nullptr, allocator)
        {
        }

        explicit HashSet(SizeType bucketCount, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(getNormalizedBucketCount(bucketCount), nullptr, allocator)
        {
        }

        HashSet(SizeType bucketCount, Hash const& hash, KeyEqual const& equal, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(getNormalizedBucketCount(bucketCount), nullptr, allocator)
            , hashFunction(hash)
            , keyEqual(equal)
        {
        }

        template <typename InputIterator, typename = Traits::EnableIfType<!Traits::isIntegral<InputIterator>>>
        HashSet(InputIterator first, InputIterator last, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(DefaultBucketCount, nullptr, allocator)
        {
            insert(first, last);
        }

        template <typename InputIterator, typename = Traits::EnableIfType<!Traits::isIntegral<InputIterator>>, typename Dummy = void>
        HashSet(InputIterator first, InputIterator last, SizeType bucketCount, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(getNormalizedBucketCount(bucketCount), nullptr, allocator)
        {
            insert(first, last);
        }

        HashSet(InitializerList<ValueType> init, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(DefaultBucketCount, nullptr, allocator)
        {
            insert(init);
        }

        HashSet(InitializerList<ValueType> init, SizeType bucketCount, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(getNormalizedBucketCount(bucketCount), nullptr, allocator)
        {
            insert(init);
        }

        HashSet(HashSet const& other)
            : allocator{other.allocator}
            , buckets(getNormalizedBucketCount(other.buckets.getCount()), nullptr, allocator)
            , maxLoad(other.maxLoad)
            , hashFunction(other.hashFunction)
            , keyEqual(other.keyEqual)
        {
            for (ConstIterator it = other.begin(); it != other.end(); ++it)
            {
                insert(*it);
            }
        }

        HashSet(HashSet&& other)
            : allocator{other.allocator}
            , buckets(DefaultBucketCount, nullptr, allocator)
        {
            swap(other);
        }

        ~HashSet()
        {
            removeAll();
        }

        HashSet& operator=(HashSet const& other)
        {
            if (this != &other)
            {
                HashSet copy(other);
                swap(copy);
            }
            return *this;
        }

        HashSet& operator=(HashSet&& other)
        {
            if (this != &other)
            {
                removeAll();
                buckets = Array<Node*>(DefaultBucketCount, nullptr, allocator);
                swap(other);
            }
            return *this;
        }

#pragma endregion

#pragma region Accessors and Modifiers

        /// Returns whether it is empty.
        bool isEmpty() const
        {
            return entryCount == 0;
        }

        /// Returns the number of stored elements.
        SizeType getCount() const
        {
            return entryCount;
        }

        /// Returns the number of stored elements.
        SizeType getCount(Key const& key) const
        {
            return contains(key) ? 1 : 0;
        }

        /// Returns load factor.
        float getLoadFactor() const
        {
            return buckets.isEmpty() ? 0.0F : static_cast<float>(entryCount) / static_cast<float>(buckets.getCount());
        }

        /// Returns max load factor.
        float getMaxLoadFactor() const
        {
            return maxLoad;
        }

        /// Sets max load factor.
        void setMaxLoadFactor(float ml)
        {
            maxLoad = (ml > 0.0F) ? ml : 1.0F;
            reserve(entryCount);
        }

        /// Reserves capacity.
        void reserve(SizeType newCap)
        {
            const SizeType minBucketCount = static_cast<SizeType>(static_cast<float>(newCap) / maxLoad) + 1;
            if (minBucketCount > buckets.getCount())
            {
                rehash(minBucketCount);
            }
        }

        /// Rebuilds buckets with a new bucket count.
        void rehash(SizeType bucketCount)
        {
            const SizeType minimumBucketCount = static_cast<SizeType>(static_cast<float>(entryCount) / maxLoad) + 1;
            const SizeType newBucketCount = Ark::max(getNormalizedBucketCount(bucketCount), minimumBucketCount);
            if (newBucketCount == buckets.getCount())
            {
                return;
            }

            Array<Node*> newBuckets(newBucketCount, nullptr, allocator);

            Node* node = orderHead;
            while (node != nullptr)
            {
                Node* nextNode = node->orderNext;
                const SizeType index = getBucketIndex(node->value, newBucketCount);
                node->bucketNext = newBuckets[index];
                newBuckets[index] = node;
                node = nextNode;
            }

            buckets = Ark::move(newBuckets);
        }

        /// Removes all elements.
        void removeAll()
        {
            Node* node = orderHead;
            while (node != nullptr)
            {
                Node* nextNode = node->orderNext;
                destroyNode(node);
                node = nextNode;
            }

            orderHead = nullptr;
            orderTail = nullptr;
            entryCount = 0;

            const SizeType bucketCount = buckets.isEmpty() ? DefaultBucketCount : buckets.getCount();
            buckets = Array<Node*>(bucketCount, nullptr, allocator);
        }

#pragma endregion

#pragma region Lookup

        /// Returns whether the element exists.
        bool contains(Key const& key) const
        {
            return findNode(key) != nullptr;
        }

        /// Attempts to get.
        Option<ConstReference> tryGet(Key const& key) const
        {
            Node* node = findNode(key);
            if (node == nullptr)
            {
                return none;
            }
            return node->value;
        }

        /// Finds an element.
        Iterator find(Key const& key)
        {
            return Iterator(findNode(key));
        }

        /// Finds an element.
        ConstIterator find(Key const& key) const
        {
            return ConstIterator(findNode(key));
        }

#pragma endregion

#pragma region Insertion and Assignment

        /// Inserts an element.
        Pair<Iterator, bool> insert(ValueType const& value)
        {
            return insertValue(value);
        }

        /// Inserts an element.
        Pair<Iterator, bool> insert(ValueType&& value)
        {
            return insertValue(Ark::move(value));
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
        void insert(InitializerList<ValueType> list)
        {
            insert(list.begin(), list.end());
        }

        /// Constructs and inserts an element.
        template <typename... Args>
        Pair<Iterator, bool> emplace(Args&&... args)
        {
            ValueType value(Ark::forward<Args>(args)...);
            return insert(Ark::move(value));
        }

#pragma endregion

#pragma region Removal

        /// Removes an element.
        SizeType remove(Key const& key)
        {
            Node* node = findNode(key);
            if (node == nullptr)
            {
                return 0;
            }

            eraseNode(node);
            return 1;
        }

        /// Removes an element.
        Iterator remove(Iterator position)
        {
            if (position.node == nullptr)
            {
                return end();
            }

            Node* next = position.node->orderNext;
            eraseNode(position.node);
            return Iterator(next);
        }

        /// Removes an element.
        Iterator remove(ConstIterator position)
        {
            if (position.node == nullptr)
            {
                return end();
            }

            Node* next = const_cast<Node*>(position.node->orderNext);
            eraseNode(const_cast<Node*>(position.node));
            return Iterator(next);
        }

#pragma endregion

#pragma region Range Queries

        /// Returns the range matching the provided key.
        Pair<Iterator, Iterator> equalRange(KeyType const& key)
        {
            Iterator first = find(key);
            if (first == end())
            {
                return {end(), end()};
            }

            Iterator last = first;
            ++last;
            return {first, last};
        }

        /// Returns the range matching the provided key.
        Pair<ConstIterator, ConstIterator> equalRange(KeyType const& key) const
        {
            ConstIterator first = find(key);
            if (first == end())
            {
                return {end(), end()};
            }

            ConstIterator last = first;
            ++last;
            return {first, last};
        }

#pragma endregion

#pragma region Bucket Interface

        /// Returns bucket.
        SizeType getBucket(KeyType const& key) const
        {
            return getBucketIndex(key, buckets.getCount());
        }

        /// Returns bucket size.
        SizeType getBucketSize(SizeType n) const
        {
            if (n >= buckets.getCount())
            {
                return 0;
            }

            SizeType result = 0;
            Node* node = buckets[n];
            while (node != nullptr)
            {
                ++result;
                node = node->bucketNext;
            }

            return result;
        }

        /// Merges entries from another container.
        void mergeWith(HashSet& other)
        {
            if (this == &other)
            {
                return;
            }

            Node* node = other.orderHead;
            while (node != nullptr)
            {
                Node* nextNode = node->orderNext;
                if (!contains(node->value))
                {
                    other.detachNode(node);
                    ensureInsertCapacity();
                    appendNode(node);
                }
                node = nextNode;
            }
        }

#pragma endregion

#pragma region Properties

        /// Returns hash function.
        HasherType getHashFunction() const
        {
            return hashFunction;
        }

        /// Returns key equal.
        KeyEqualType getKeyEqual() const
        {
            return keyEqual;
        }

        /// Returns the allocator used by this container.
        AllocatorType getAllocator() const
        {
            return &allocator;
        }

#pragma endregion

#pragma region Iterators

        /// Returns an iterator to the first element.
        Iterator getStartIterator()
        {
            return Iterator(orderHead);
        }

        /// Returns an iterator to the first element.
        ConstIterator getStartIterator() const
        {
            return ConstIterator(orderHead);
        }

        /// Returns an iterator to one-past-the-last element.
        Iterator getEndIterator()
        {
            return Iterator(nullptr);
        }

        /// Returns an iterator to one-past-the-last element.
        ConstIterator getEndIterator() const
        {
            return ConstIterator(nullptr);
        }

        /// Returns start bucket iterator.
        LocalIterator getStartBucketIterator(SizeType n)
        {
            return (n < buckets.getCount()) ? LocalIterator(buckets[n]) : LocalIterator(nullptr);
        }

        /// Returns start bucket iterator.
        ConstLocalIterator getStartBucketIterator(SizeType n) const
        {
            return (n < buckets.getCount()) ? ConstLocalIterator(buckets[n]) : ConstLocalIterator(nullptr);
        }

        /// Returns end bucket iterator.
        LocalIterator getEndBucketIterator(SizeType n)
        {
            (void)n;
            return LocalIterator(nullptr);
        }

        /// Returns end bucket iterator.
        ConstLocalIterator getEndBucketIterator(SizeType n) const
        {
            (void)n;
            return ConstLocalIterator(nullptr);
        }

#pragma endregion

#pragma region Operators

        /// Swaps contents with another object.
        void swap(HashSet& other)
        {
            if (&allocator != &other.allocator)
            {
                HashSet thisCopy(*this);
                HashSet otherCopy(other);

                removeAll();
                other.removeAll();

                maxLoad = otherCopy.maxLoad;
                hashFunction = otherCopy.hashFunction;
                keyEqual = otherCopy.keyEqual;

                other.maxLoad = thisCopy.maxLoad;
                other.hashFunction = thisCopy.hashFunction;
                other.keyEqual = thisCopy.keyEqual;

                reserve(otherCopy.getCount());
                for (ConstIterator it = otherCopy.begin(); it != otherCopy.end(); ++it)
                {
                    insert(*it);
                }

                other.reserve(thisCopy.getCount());
                for (ConstIterator it = thisCopy.begin(); it != thisCopy.end(); ++it)
                {
                    other.insert(*it);
                }
                return;
            }

            Ark::swap(buckets, other.buckets);
            Ark::swap(orderHead, other.orderHead);
            Ark::swap(orderTail, other.orderTail);
            Ark::swap(entryCount, other.entryCount);
            Ark::swap(maxLoad, other.maxLoad);
            Ark::swap(hashFunction, other.hashFunction);
            Ark::swap(keyEqual, other.keyEqual);
        }

#pragma endregion

#pragma region STL Compatibility API

    public:
        using key_type = KeyType;
        using value_type = ValueType;
        using size_type = SizeType;
        using difference_type = DifferenceType;
        using hasher = HasherType;
        using key_equal = KeyEqualType;
        using allocator_type = AllocatorType;
        using reference = Reference;
        using const_reference = ConstReference;
        using pointer = Pointer;
        using const_pointer = ConstPointer;
        using iterator = Iterator;
        using const_iterator = ConstIterator;
        using local_iterator = LocalIterator;
        using const_local_iterator = ConstLocalIterator;

        SizeType size() const
        {
            return getCount();
        }

        bool empty() const
        {
            return isEmpty();
        }

        float load_factor() const
        {
            return getLoadFactor();
        }

        float max_load_factor() const
        {
            return getMaxLoadFactor();
        }

        void max_load_factor(float ml)
        {
            setMaxLoadFactor(ml);
        }

        size_type count(Key const& key) const
        {
            return getCount(key);
        }

        size_type bucket_count() const
        {
            return buckets.getCount();
        }

        size_type bucket_size(size_type n) const
        {
            return getBucketSize(n);
        }

        size_type bucket(Key const& key) const
        {
            return getBucket(key);
        }

        hasher hash_function() const
        {
            return getHashFunction();
        }

        key_equal key_eq() const
        {
            return getKeyEqual();
        }

        allocator_type get_allocator() const
        {
            return getAllocator();
        }

        void clear()
        {
            removeAll();
        }

        SizeType erase(Key const& key)
        {
            return remove(key);
        }

        Iterator begin()
        {
            return getStartIterator();
        }

        ConstIterator begin() const
        {
            return getStartIterator();
        }

        ConstIterator cbegin() const
        {
            return getStartIterator();
        }

        Iterator end()
        {
            return getEndIterator();
        }

        ConstIterator end() const
        {
            return getEndIterator();
        }

        ConstIterator cend() const
        {
            return getEndIterator();
        }

        LocalIterator begin(SizeType n)
        {
            return getStartBucketIterator(n);
        }

        ConstLocalIterator begin(SizeType n) const
        {
            return getStartBucketIterator(n);
        }

        ConstLocalIterator cbegin(SizeType n) const
        {
            return getStartBucketIterator(n);
        }

        LocalIterator end(SizeType n)
        {
            return getEndBucketIterator(n);
        }

        ConstLocalIterator end(SizeType n) const
        {
            return getEndBucketIterator(n);
        }

        ConstLocalIterator cend(SizeType n) const
        {
            return getEndBucketIterator(n);
        }

        Iterator erase(Iterator position)
        {
            return remove(position);
        }

        Iterator erase(ConstIterator position)
        {
            return remove(position);
        }

        void merge(HashSet& source)
        {
            mergeWith(source);
        }

        void merge(HashSet&& source)
        {
            mergeWith(source);
        }

        template <typename... Args>
        Iterator emplace_hint(ConstIterator hint, Args&&... args)
        {
            (void)hint;
            return emplace(Ark::forward<Args>(args)...).first;
        }

#pragma endregion

    private:
        /// Returns normalized bucket count.
        static SizeType getNormalizedBucketCount(SizeType bucketCount)
        {
            return bucketCount == 0 ? DefaultBucketCount : bucketCount;
        }

        /// Returns bucket index.
        SizeType getBucketIndex(Key const& key, SizeType bucketCount) const
        {
            return bucketCount == 0 ? 0 : static_cast<SizeType>(hashFunction(key) % bucketCount);
        }

        /// Ensures enough capacity for insertion.
        void ensureInsertCapacity()
        {
            if (buckets.isEmpty())
            {
                buckets = Array<Node*>(DefaultBucketCount, nullptr, allocator);
            }

            const float nextLoad = static_cast<float>(entryCount + 1) / static_cast<float>(buckets.getCount());
            if (nextLoad > maxLoad)
            {
                rehash(buckets.getCount() * 2);
            }
        }

        /// Finds node.
        Node* findNode(Key const& key) const
        {
            if (buckets.isEmpty())
            {
                return nullptr;
            }

            Node* node = buckets[getBucketIndex(key, buckets.getCount())];
            while (node != nullptr)
            {
                if (keyEqual(node->value, key))
                {
                    return node;
                }
                node = node->bucketNext;
            }

            return nullptr;
        }

        /// Creates a node.
        template <typename K>
        Node* createNode(K&& key)
        {
            return allocator.make<Node>(Ark::forward<K>(key));
        }

        /// Creates a node from a value.
        Node* createNodeFromValue(ValueType const& value)
        {
            return allocator.make<Node>(value);
        }

        /// Creates a node from a value.
        Node* createNodeFromValue(ValueType&& value)
        {
            return allocator.make<Node>(Ark::move(value));
        }

        /// Destroys a node.
        void destroyNode(Node* node)
        {
            allocator.destroy(node);
        }

        /// Appends a node to the linked order.
        void appendNode(Node* node)
        {
            const SizeType bucketIndex = getBucketIndex(node->value, buckets.getCount());
            node->bucketNext = buckets[bucketIndex];
            buckets[bucketIndex] = node;

            node->orderPrev = orderTail;
            node->orderNext = nullptr;
            if (orderTail != nullptr)
            {
                orderTail->orderNext = node;
            }
            else
            {
                orderHead = node;
            }
            orderTail = node;
            ++entryCount;
        }

        /// Detaches a node from the linked order.
        void detachNode(Node* node)
        {
            const SizeType bucketIndex = getBucketIndex(node->value, buckets.getCount());
            Node* prevBucket = nullptr;
            Node* bucketNode = buckets[bucketIndex];
            while (bucketNode != nullptr)
            {
                if (bucketNode == node)
                {
                    if (prevBucket != nullptr)
                    {
                        prevBucket->bucketNext = bucketNode->bucketNext;
                    }
                    else
                    {
                        buckets[bucketIndex] = bucketNode->bucketNext;
                    }
                    break;
                }
                prevBucket = bucketNode;
                bucketNode = bucketNode->bucketNext;
            }

            if (node->orderPrev != nullptr)
            {
                node->orderPrev->orderNext = node->orderNext;
            }
            else
            {
                orderHead = node->orderNext;
            }

            if (node->orderNext != nullptr)
            {
                node->orderNext->orderPrev = node->orderPrev;
            }
            else
            {
                orderTail = node->orderPrev;
            }

            node->bucketNext = nullptr;
            node->orderPrev = nullptr;
            node->orderNext = nullptr;
            --entryCount;
        }

        /// Removes node.
        void eraseNode(Node* node)
        {
            detachNode(node);
            destroyNode(node);
        }

        /// Inserts value.
        Pair<Iterator, bool> insertValue(ValueType const& value)
        {
            Node* existing = findNode(value);
            if (existing != nullptr)
            {
                return {Iterator(existing), false};
            }

            ensureInsertCapacity();
            Node* node = createNodeFromValue(value);
            appendNode(node);
            return {Iterator(node), true};
        }

        /// Inserts value.
        Pair<Iterator, bool> insertValue(ValueType&& value)
        {
            Node* existing = findNode(value);
            if (existing != nullptr)
            {
                return {Iterator(existing), false};
            }

            ensureInsertCapacity();
            Node* node = createNodeFromValue(Ark::move(value));
            appendNode(node);
            return {Iterator(node), true};
        }
    };

    /// Swaps contents with another object.
    template <typename Key, typename Hash, typename KeyEqual>
    inline void swap(HashSet<Key, Hash, KeyEqual>& a, HashSet<Key, Hash, KeyEqual>& b)
    {
        a.swap(b);
    }
}
