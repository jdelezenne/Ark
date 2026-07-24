#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Hasher.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections
{
    namespace Internal
    {
        template <typename Key>
        struct LruCacheDefaultHash final
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
                    static_assert(Traits::isSame<Key, void>, "LruCache requires a hash function for this key type.");
                    return 0;
                }
            }
        };
    }

    /// An LRU cache with O(1) insert/get/remove. MRU is at the head; LRU is at the tail.
    /// @tparam Key Key type.
    /// @tparam T Mapped value type.
    /// @tparam Hash Hash functor.
    /// @tparam KeyEqual Key equality functor.
    template <typename Key, typename T, typename Hash = Internal::LruCacheDefaultHash<Key>, typename KeyEqual = EqualTo<Key>>
    struct LruCache final
    {
    private:
        using KeyStorageType = Traits::RemoveCVType<Key>;

        struct Node
        {
            Pair<KeyStorageType, T> value;
            Node* bucketNext{nullptr};
            Node* orderPrev{nullptr};
            Node* orderNext{nullptr};

            template <typename K, typename V>
            Node(K&& key, V&& mapped)
                : value(Ark::forward<K>(key), Ark::forward<V>(mapped))
            {
            }
        };

        static constexpr usize DefaultBucketCount = 16;

        Memory::Allocator& allocator{Memory::getDefaultAllocator()};
        Array<Node*> buckets;
        Node* orderHead{nullptr};
        Node* orderTail{nullptr};
        usize entryCount{0};
        usize capacity{0};
        Hash hashFunction{};
        KeyEqual keyEqual{};

    public:
#pragma region Types

        using KeyType = Key;
        using MappedType = T;
        using ValueType = Pair<KeyStorageType, T>;
        using SizeType = usize;
        using DifferenceType = isize;
        using HasherType = Hash;
        using KeyEqualType = KeyEqual;
        using AllocatorType = Memory::Allocator*;
        using Reference = ValueType&;
        using ConstReference = ValueType const&;
        using Pointer = ValueType*;
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

#pragma endregion

#pragma region Constructors and Assignments

        LruCache(Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(DefaultBucketCount, nullptr, allocator)
            , capacity{1}
        {
        }

        explicit LruCache(SizeType maxCapacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
            : allocator{allocator}
            , buckets(DefaultBucketCount, nullptr, allocator)
            , capacity{maxCapacity}
        {
            ARK_ASSERT(maxCapacity > 0);
        }

        LruCache(LruCache const& other)
            : allocator{other.allocator}
            , buckets(DefaultBucketCount, nullptr, allocator)
            , capacity{other.capacity}
            , hashFunction{other.hashFunction}
            , keyEqual{other.keyEqual}
        {
            for (ConstIterator it = other.getStartIterator(); it != other.getEndIterator(); ++it)
            {
                insert(it->first, it->second);
            }
        }

        LruCache(LruCache&& other)
            : allocator{other.allocator}
            , buckets(DefaultBucketCount, nullptr, allocator)
        {
            swap(other);
        }

        ~LruCache()
        {
            removeAll();
        }

        LruCache& operator=(LruCache const& other)
        {
            if (this != &other)
            {
                removeAll();
                capacity = other.capacity;
                hashFunction = other.hashFunction;
                keyEqual = other.keyEqual;
                for (ConstIterator it = other.getStartIterator(); it != other.getEndIterator(); ++it)
                {
                    insert(it->first, it->second);
                }
            }
            return *this;
        }

        LruCache& operator=(LruCache&& other)
        {
            if (this != &other)
            {
                removeAll();
                swap(other);
            }
            return *this;
        }

        /// Constructs a cache with the given maximum capacity.
        static LruCache withCapacity(SizeType maxCapacity, Memory::Allocator& allocator = Memory::getDefaultAllocator())
        {
            return LruCache(maxCapacity, allocator);
        }

#pragma endregion

#pragma region Accessors

        SizeType getCapacity() const
        {
            return capacity;
        }

        SizeType getCount() const
        {
            return entryCount;
        }

        bool isEmpty() const
        {
            return entryCount == 0;
        }

        bool contains(Key const& key) const
        {
            return findNode(key) != nullptr;
        }

        bool hasKey(Key const& key) const
        {
            return contains(key);
        }

        /// Returns a reference to the value and promotes the entry to MRU.
        Option<MappedType&> tryGet(Key const& key)
        {
            Node* node = findNode(key);
            if (node == nullptr)
            {
                return none;
            }
            promote(node);
            return node->value.second;
        }

        /// Returns a const reference without changing recency (same as peek).
        Option<MappedType const&> tryGet(Key const& key) const
        {
            return peek(key);
        }

        /// Returns a reference to the value without changing recency order.
        Option<MappedType&> peek(Key const& key)
        {
            Node* node = findNode(key);
            if (node == nullptr)
            {
                return none;
            }
            return node->value.second;
        }

        /// Returns a const reference to the value without changing recency order.
        Option<MappedType const&> peek(Key const& key) const
        {
            Node const* node = findNode(key);
            if (node == nullptr)
            {
                return none;
            }
            return node->value.second;
        }

        AllocatorType getAllocator() const
        {
            return &allocator;
        }

#pragma endregion

#pragma region Iterators

        /// Returns an iterator to the MRU entry.
        Iterator getStartIterator()
        {
            return Iterator(orderHead);
        }

        ConstIterator getStartIterator() const
        {
            return ConstIterator(orderHead);
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

        /// Inserts or updates a key-value pair. Promotes the entry to MRU.
        /// @return The previous value if the key existed, otherwise none.
        Option<MappedType> insert(Key const& key, MappedType const& value)
        {
            return insertImpl(KeyStorageType(key), MappedType(value));
        }

        /// Inserts or updates a key-value pair. Promotes the entry to MRU.
        /// @return The previous value if the key existed, otherwise none.
        Option<MappedType> insert(Key&& key, MappedType&& value)
        {
            return insertImpl(Ark::move(key), Ark::move(value));
        }

        /// Inserts or updates a key-value pair. Returns an evicted or replaced entry when applicable.
        /// @return Previous entry for the same key, or the LRU entry evicted to make room, otherwise none.
        Option<ValueType> insertOrGetEvicted(Key const& key, MappedType const& value)
        {
            return insertOrGetEvictedImpl(KeyStorageType(key), MappedType(value));
        }

        /// Inserts or updates a key-value pair. Returns an evicted or replaced entry when applicable.
        Option<ValueType> insertOrGetEvicted(Key&& key, MappedType&& value)
        {
            return insertOrGetEvictedImpl(Ark::move(key), Ark::move(value));
        }

        /// Removes and returns the value for the key, or none if missing.
        Option<MappedType> remove(Key const& key)
        {
            Node* node = findNode(key);
            if (node == nullptr)
            {
                return none;
            }

            MappedType value = Ark::move(node->value.second);
            eraseNode(node);
            return value;
        }

        /// Removes and returns the least-recently-used entry, or none if empty.
        Option<ValueType> removeLeastRecentlyUsed()
        {
            if (orderTail == nullptr)
            {
                return none;
            }

            Node* node = orderTail;
            ValueType value = Ark::move(node->value);
            eraseNode(node);
            return value;
        }

        void removeAll()
        {
            Node* node = orderHead;
            while (node != nullptr)
            {
                Node* next = node->orderNext;
                allocator.destroy(node);
                node = next;
            }

            orderHead = nullptr;
            orderTail = nullptr;
            entryCount = 0;
            buckets = Array<Node*>(DefaultBucketCount, nullptr, allocator);
        }

        /// Sets the maximum capacity. Evicts LRU entries if the new capacity is smaller.
        void setCapacity(SizeType maxCapacity)
        {
            ARK_ASSERT(maxCapacity > 0);
            capacity = maxCapacity;
            while (entryCount > capacity)
            {
                (void)removeLeastRecentlyUsed();
            }
        }

        void swap(LruCache& other)
        {
            if (this == &other)
            {
                return;
            }

            if (&allocator != &other.allocator)
            {
                LruCache thisCopy(*this);
                LruCache otherCopy(other);
                *this = Ark::move(otherCopy);
                other = Ark::move(thisCopy);
                return;
            }

            Array<Node*> tmpBuckets = Ark::move(buckets);
            buckets = Ark::move(other.buckets);
            other.buckets = Ark::move(tmpBuckets);

            Node* tmpHead = orderHead;
            Node* tmpTail = orderTail;
            usize tmpCount = entryCount;
            usize tmpCapacity = capacity;
            Hash tmpHash = hashFunction;
            KeyEqual tmpEqual = keyEqual;

            orderHead = other.orderHead;
            orderTail = other.orderTail;
            entryCount = other.entryCount;
            capacity = other.capacity;
            hashFunction = other.hashFunction;
            keyEqual = other.keyEqual;

            other.orderHead = tmpHead;
            other.orderTail = tmpTail;
            other.entryCount = tmpCount;
            other.capacity = tmpCapacity;
            other.hashFunction = tmpHash;
            other.keyEqual = tmpEqual;
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

        size_type cap() const
        {
            return getCapacity();
        }

        void clear()
        {
            removeAll();
        }

#pragma endregion

    private:
        SizeType getBucketIndex(Key const& key, SizeType bucketCount) const
        {
            return bucketCount == 0 ? 0 : static_cast<SizeType>(hashFunction(key) % bucketCount);
        }

        Node* findNode(Key const& key) const
        {
            if (buckets.isEmpty())
            {
                return nullptr;
            }

            SizeType index = getBucketIndex(key, buckets.getCount());
            Node* node = buckets[index];
            while (node != nullptr)
            {
                if (keyEqual(node->value.first, key))
                {
                    return node;
                }
                node = node->bucketNext;
            }
            return nullptr;
        }

        void ensureBuckets()
        {
            if (static_cast<float>(entryCount + 1) > static_cast<float>(buckets.getCount()))
            {
                rehash(buckets.getCount() * 2);
            }
        }

        void rehash(SizeType newBucketCount)
        {
            if (newBucketCount < DefaultBucketCount)
            {
                newBucketCount = DefaultBucketCount;
            }

            Array<Node*> newBuckets(newBucketCount, nullptr, allocator);
            Node* node = orderHead;
            while (node != nullptr)
            {
                Node* next = node->orderNext;
                SizeType index = getBucketIndex(node->value.first, newBucketCount);
                node->bucketNext = newBuckets[index];
                newBuckets[index] = node;
                node = next;
            }
            buckets = Ark::move(newBuckets);
        }

        void linkBucket(Node* node)
        {
            SizeType index = getBucketIndex(node->value.first, buckets.getCount());
            node->bucketNext = buckets[index];
            buckets[index] = node;
        }

        void unlinkBucket(Node* node)
        {
            SizeType index = getBucketIndex(node->value.first, buckets.getCount());
            Node* prev = nullptr;
            Node* current = buckets[index];
            while (current != nullptr)
            {
                if (current == node)
                {
                    if (prev != nullptr)
                    {
                        prev->bucketNext = current->bucketNext;
                    }
                    else
                    {
                        buckets[index] = current->bucketNext;
                    }
                    break;
                }
                prev = current;
                current = current->bucketNext;
            }
            node->bucketNext = nullptr;
        }

        void linkFront(Node* node)
        {
            node->orderPrev = nullptr;
            node->orderNext = orderHead;
            if (orderHead != nullptr)
            {
                orderHead->orderPrev = node;
            }
            else
            {
                orderTail = node;
            }
            orderHead = node;
            ++entryCount;
        }

        void unlinkOrder(Node* node)
        {
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

            node->orderPrev = nullptr;
            node->orderNext = nullptr;
            --entryCount;
        }

        void promote(Node* node)
        {
            if (node == orderHead)
            {
                return;
            }
            unlinkOrder(node);
            linkFront(node);
        }

        void eraseNode(Node* node)
        {
            unlinkBucket(node);
            unlinkOrder(node);
            allocator.destroy(node);
        }

        template <typename K, typename V>
        Option<MappedType> insertImpl(K&& key, V&& value)
        {
            Node* existing = findNode(key);
            if (existing != nullptr)
            {
                MappedType old = Ark::move(existing->value.second);
                existing->value.second = Ark::forward<V>(value);
                promote(existing);
                return old;
            }

            if (entryCount >= capacity)
            {
                (void)removeLeastRecentlyUsed();
            }

            ensureBuckets();
            Node* node = allocator.make<Node>(Ark::forward<K>(key), Ark::forward<V>(value));
            linkBucket(node);
            linkFront(node);
            return none;
        }

        template <typename K, typename V>
        Option<ValueType> insertOrGetEvictedImpl(K&& key, V&& value)
        {
            Node* existing = findNode(key);
            if (existing != nullptr)
            {
                ValueType old = Ark::move(existing->value);
                existing->value.first = old.first;
                existing->value.second = Ark::forward<V>(value);
                promote(existing);
                return old;
            }

            Option<ValueType> evicted = none;
            if (entryCount >= capacity)
            {
                evicted = removeLeastRecentlyUsed();
            }

            ensureBuckets();
            Node* node = allocator.make<Node>(Ark::forward<K>(key), Ark::forward<V>(value));
            linkBucket(node);
            linkFront(node);
            return evicted;
        }
    };
}
