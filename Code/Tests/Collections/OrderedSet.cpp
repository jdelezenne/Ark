#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/OrderedSet.hpp"

ARK_TEST_CASE("OrderedSet keeps key order and enforces uniqueness", "[collections][orderedset]")
{
    using Ark::Collections::OrderedSet;

    OrderedSet<int> set;
    auto firstInsert = set.insert(5);
    auto secondInsert = set.insert(1);
    auto duplicateInsert = set.insert(5);

    REQUIRE(firstInsert.second);
    REQUIRE(secondInsert.second);
    REQUIRE(!duplicateInsert.second);
    REQUIRE(set.getCount() == 2);

    auto iterator = set.begin();
    REQUIRE(iterator != set.end());
    REQUIRE(*iterator == 1);
    ++iterator;
    REQUIRE(iterator != set.end());
    REQUIRE(*iterator == 5);
}

ARK_TEST_CASE("OrderedSet lower upper and equal range", "[collections][orderedset]")
{
    using Ark::Collections::OrderedSet;

    OrderedSet<int> set;
    set.insert(10);
    set.insert(20);
    set.insert(30);

    auto lower = set.lowerBound(15);
    REQUIRE(lower != set.end());
    REQUIRE(*lower == 20);

    auto upper = set.upperBound(20);
    REQUIRE(upper != set.end());
    REQUIRE(*upper == 30);

    auto range = set.equalRange(20);
    REQUIRE(range.first != set.end());
    REQUIRE(*range.first == 20);
    REQUIRE(range.second != set.end());
    REQUIRE(*range.second == 30);
}
