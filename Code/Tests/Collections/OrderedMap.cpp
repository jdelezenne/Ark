#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/OrderedMap.hpp"

ARK_TEST_CASE("OrderedMap keeps key order and supports assignment helpers", "[collections][orderedmap]")
{
    using Ark::Collections::OrderedMap;

    OrderedMap<int, int> map;
    map.insert({3, 30});
    map.insert({1, 10});
    map.insertOrAssign(2, 20);
    map.insertOrAssign(2, 200);

    REQUIRE(map.getCount() == 3);
    REQUIRE(map.getAt(2) == 200);

    auto iterator = map.begin();
    REQUIRE(iterator != map.end());
    REQUIRE(iterator->first == 1);
    ++iterator;
    REQUIRE(iterator != map.end());
    REQUIRE(iterator->first == 2);
    ++iterator;
    REQUIRE(iterator != map.end());
    REQUIRE(iterator->first == 3);
}

ARK_TEST_CASE("OrderedMap lower upper and equal range", "[collections][orderedmap]")
{
    using Ark::Collections::OrderedMap;

    OrderedMap<int, int> map;
    map.insert({10, 100});
    map.insert({20, 200});
    map.insert({30, 300});

    auto lower = map.lowerBound(15);
    REQUIRE(lower != map.end());
    REQUIRE(lower->first == 20);

    auto upper = map.upperBound(20);
    REQUIRE(upper != map.end());
    REQUIRE(upper->first == 30);

    auto range = map.equalRange(20);
    REQUIRE(range.first != map.end());
    REQUIRE(range.first->first == 20);
    REQUIRE(range.second != map.end());
    REQUIRE(range.second->first == 30);
}
