#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/HashSet.hpp"

ARK_TEST_CASE("HashSet insert duplicate and lookup semantics", "[collections][hashset]")
{
    using Ark::Collections::HashSet;

    HashSet<int> set;
    REQUIRE(set.isEmpty());

    auto inserted = set.insert(7);
    REQUIRE(inserted.second);
    REQUIRE(set.getCount() == 1);
    REQUIRE(set.contains(7));

    auto insertedAgain = set.insert(7);
    REQUIRE(!insertedAgain.second);
    REQUIRE(set.getCount() == 1);

    auto found = set.find(7);
    REQUIRE(found != set.end());
    REQUIRE(*found == 7);

    auto missing = set.find(99);
    REQUIRE(missing == set.end());

    auto value = set.tryGet(7);
    REQUIRE(value.hasValue());
    REQUIRE(value.getValue() == 7);
}

ARK_TEST_CASE("HashSet erase and reserve preserve remaining element addresses", "[collections][hashset]")
{
    using Ark::Collections::HashSet;

    HashSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);

    auto iterTwo = set.find(2);
    REQUIRE(iterTwo != set.end());

    auto valueTwoAddress = &(*iterTwo);

    auto iterOne = set.find(1);
    REQUIRE(iterOne != set.end());

    auto next = set.remove(iterOne);
    REQUIRE(set.getCount() == 2);
    REQUIRE(!set.contains(1));
    const bool isValidNext = (next == set.end()) || (*next == 2) || (*next == 3);
    REQUIRE(isValidNext);

    auto iterTwoAfterErase = set.find(2);
    REQUIRE(iterTwoAfterErase != set.end());
    REQUIRE(*iterTwoAfterErase == 2);
    REQUIRE(&(*iterTwoAfterErase) == valueTwoAddress);

    set.reserve(512);

    auto iterTwoAfterReserve = set.find(2);
    REQUIRE(iterTwoAfterReserve != set.end());
    REQUIRE(*iterTwoAfterReserve == 2);
    REQUIRE(&(*iterTwoAfterReserve) == valueTwoAddress);
}

ARK_TEST_CASE("HashSet remove by key and equalRange", "[collections][hashset]")
{
    using Ark::Collections::HashSet;

    HashSet<int> set;
    set.insert(4);
    set.insert(8);

    REQUIRE(set.remove(4) == 1);
    REQUIRE(!set.contains(4));
    REQUIRE(set.remove(4) == 0);

    auto existing = set.equalRange(8);
    REQUIRE(existing.first != set.end());
    REQUIRE(*existing.first == 8);

    auto missing = set.equalRange(999);
    REQUIRE(missing.first == set.end());
    REQUIRE(missing.second == set.end());
}

ARK_TEST_CASE("HashSet STL compatibility API", "[collections][hashset]")
{
    using Ark::Collections::HashSet;

    HashSet<int> set;
    set.emplace_hint(set.cbegin(), 1);
    set.emplace_hint(set.cbegin(), 2);

    REQUIRE(set.size() == 2);
    REQUIRE(!set.empty());
    REQUIRE(set.count(1) == 1);
    REQUIRE(set.load_factor() > 0.0F);
    REQUIRE(set.max_load_factor() == 1.0F);

    set.max_load_factor(0.5F);
    REQUIRE(set.max_load_factor() == 0.5F);

    const auto bucketCount = set.bucket_count();
    REQUIRE(bucketCount > 0);
    REQUIRE(set.bucket(1) < bucketCount);
    REQUIRE(set.bucket_size(set.bucket(1)) >= 1);

    auto hasher = set.hash_function();
    REQUIRE(hasher(1) == hasher(1));

    auto eq = set.key_eq();
    REQUIRE(eq(1, 1));
    REQUIRE(!eq(1, 2));

    auto allocator = set.get_allocator();
    REQUIRE(allocator != nullptr);

    HashSet<int> other;
    other.insert(2);
    other.insert(3);
    set.merge(other);
    REQUIRE(set.contains(3));
    REQUIRE(other.contains(2));
    REQUIRE(!other.contains(3));
}
