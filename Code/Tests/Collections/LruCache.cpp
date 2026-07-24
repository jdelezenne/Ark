#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/LruCache.hpp"

ARK_TEST_CASE("LruCache insert update and capacity eviction", "[collections][lrucache]")
{
    using Ark::Collections::LruCache;

    auto cache = LruCache<int, int>::withCapacity(2);
    REQUIRE(cache.getCapacity() == 2);
    REQUIRE(cache.isEmpty());

    REQUIRE(!cache.insert(1, 10).hasValue());
    REQUIRE(!cache.insert(2, 20).hasValue());
    REQUIRE(cache.getCount() == 2);

    auto old = cache.insert(2, 21);
    REQUIRE(old.hasValue());
    REQUIRE(old.getValue() == 20);
    REQUIRE(cache.getCount() == 2);

    REQUIRE(!cache.insert(3, 30).hasValue());
    REQUIRE(cache.getCount() == 2);
    REQUIRE(!cache.contains(1));
    REQUIRE(cache.contains(2));
    REQUIRE(cache.contains(3));
}

ARK_TEST_CASE("LruCache tryGet promotes and peek does not", "[collections][lrucache]")
{
    using Ark::Collections::LruCache;

    auto cache = LruCache<int, int>::withCapacity(2);
    cache.insert(1, 10);
    cache.insert(2, 20);

    auto value = cache.tryGet(1);
    REQUIRE(value.hasValue());
    REQUIRE(value.getValue() == 10);

    // 1 is now MRU; inserting 3 should evict 2 (LRU), not 1.
    cache.insert(3, 30);
    REQUIRE(cache.contains(1));
    REQUIRE(!cache.contains(2));
    REQUIRE(cache.contains(3));

    cache.insert(4, 40);
    // peek must not promote 1; after insert(5) with capacity 2 and order MRU=4,LRU=1...
    // Rebuild: after insert(4), 3 was LRU and may be gone. Start clean for peek.
    cache.removeAll();
    cache.setCapacity(2);
    cache.insert(1, 10);
    cache.insert(2, 20);

    auto peeked = cache.peek(1);
    REQUIRE(peeked.hasValue());
    REQUIRE(peeked.getValue() == 10);

    cache.insert(3, 30);
    // peek did not promote 1, so 1 is still LRU and should be evicted.
    REQUIRE(!cache.contains(1));
    REQUIRE(cache.contains(2));
    REQUIRE(cache.contains(3));
}

ARK_TEST_CASE("LruCache remove and removeLeastRecentlyUsed", "[collections][lrucache]")
{
    using Ark::Collections::LruCache;

    auto cache = LruCache<int, int>::withCapacity(3);
    cache.insert(1, 10);
    cache.insert(2, 20);
    cache.insert(3, 30);

    auto removed = cache.remove(2);
    REQUIRE(removed.hasValue());
    REQUIRE(removed.getValue() == 20);
    REQUIRE(!cache.contains(2));

    auto lru = cache.removeLeastRecentlyUsed();
    REQUIRE(lru.hasValue());
    REQUIRE(lru.getValue().first == 1);
    REQUIRE(lru.getValue().second == 10);
}

ARK_TEST_CASE("LruCache iteration is MRU to LRU", "[collections][lrucache]")
{
    using Ark::Collections::LruCache;

    auto cache = LruCache<int, int>::withCapacity(3);
    cache.insert(1, 10);
    cache.insert(2, 20);
    cache.insert(3, 30);

    int keys[3] = {};
    int i = 0;
    for (auto const& entry : cache)
    {
        keys[i++] = entry.first;
    }
    REQUIRE(i == 3);
    REQUIRE(keys[0] == 3);
    REQUIRE(keys[1] == 2);
    REQUIRE(keys[2] == 1);
}
