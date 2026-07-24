#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/HashMap.hpp"

ARK_TEST_CASE("HashMap insert and duplicate semantics", "[collections][hashmap]")
{
    using Ark::Collections::HashMap;

    HashMap<int, int> map;
    REQUIRE(map.isEmpty());

    auto inserted = map.insert({1, 10});
    REQUIRE(inserted.second);
    REQUIRE(map.getCount() == 1);
    REQUIRE(map.contains(1));
    REQUIRE(map.getAt(1) == 10);

    auto insertedAgain = map.insert({1, 11});
    REQUIRE(!insertedAgain.second);
    REQUIRE(map.getAt(1) == 10);
}

ARK_TEST_CASE("HashMap operator[] and lookup helpers", "[collections][hashmap]")
{
    using Ark::Collections::HashMap;

    HashMap<int, int> map;
    map.insert({1, 10});
    map[2] = 20;

    REQUIRE(map.getCount() == 2);
    REQUIRE(map.contains(2));
    REQUIRE(map.getAt(2) == 20);

    auto existing = map.tryGet(1);
    REQUIRE(existing.hasValue());
    REQUIRE(existing.getValue() == 10);

    auto missing = map.tryGet(999);
    REQUIRE(!missing.hasValue());

    int fallbackRef = 42;
    REQUIRE(map.getOrDefault(999, fallbackRef) == 42);

    REQUIRE(map.getValueOr(1, 99) == 10);
    REQUIRE(map.getValueOr(999, 99) == 99);
}

ARK_TEST_CASE("HashMap erase and reference stability semantics", "[collections][hashmap]")
{
    using Ark::Collections::HashMap;

    HashMap<int, int> map;
    map.insert({1, 10});
    map.insert({2, 20});
    map.insert({3, 30});

    auto iterOne = map.find(1);
    auto iterTwo = map.find(2);

    REQUIRE(iterOne != map.end());
    REQUIRE(iterTwo != map.end());

    auto valueTwoAddress = &iterTwo->second;

    auto next = map.remove(iterOne);
    REQUIRE(map.getCount() == 2);
    REQUIRE(!map.contains(1));
    const bool isValidNext = (next == map.end()) || (next->first == 2) || (next->first == 3);
    REQUIRE(isValidNext);

    auto iterTwoAfterErase = map.find(2);
    REQUIRE(iterTwoAfterErase != map.end());
    REQUIRE(iterTwoAfterErase->second == 20);
    REQUIRE(&iterTwoAfterErase->second == valueTwoAddress);

    map.reserve(512);

    auto iterTwoAfterReserve = map.find(2);
    REQUIRE(iterTwoAfterReserve != map.end());
    REQUIRE(iterTwoAfterReserve->second == 20);
    REQUIRE(&iterTwoAfterReserve->second == valueTwoAddress);
}
