#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/StableArray.hpp"

ARK_TEST_CASE("StableArray append returns stable indices", "[collections][stablearray]")
{
    using Ark::Collections::StableArray;

    StableArray<int> array;
    auto a = array.append(10);
    auto b = array.append(20);
    auto c = array.append(30);

    REQUIRE(a == 0);
    REQUIRE(b == 1);
    REQUIRE(c == 2);
    REQUIRE(array.getCount() == 3);
    REQUIRE(array.get(a) == 10);
    REQUIRE(array.get(b) == 20);
    REQUIRE(array.get(c) == 30);
}

ARK_TEST_CASE("StableArray remove keeps other indices stable", "[collections][stablearray]")
{
    using Ark::Collections::StableArray;

    StableArray<char> array;
    auto star = array.append('*');
    auto heart = array.append('H');
    auto lambda = array.append('L');

    auto removed = array.remove(star);
    REQUIRE(removed.hasValue());
    REQUIRE(removed.getValue() == '*');
    REQUIRE(!array.has(star));
    REQUIRE(array.get(heart) == 'H');
    REQUIRE(array.get(lambda) == 'L');
    REQUIRE(array.getCount() == 2);

    auto previous = array.insert(star, 'S');
    REQUIRE(!previous.hasValue());
    REQUIRE(array.get(star) == 'S');
    REQUIRE(array.get(heart) == 'H');
    REQUIRE(array.get(lambda) == 'L');
    REQUIRE(array.getCount() == 3);
}

ARK_TEST_CASE("StableArray tryGet and iterator skip vacant slots", "[collections][stablearray]")
{
    using Ark::Collections::StableArray;

    StableArray<int> array;
    array.append(1);
    array.append(2);
    array.append(3);
    (void)array.remove(1);

    REQUIRE(!array.tryGet(1).hasValue());
    REQUIRE(array.tryGet(0).hasValue());
    REQUIRE(array.tryGet(0).getValue() == 1);

    int values[2] = {};
    Ark::usize indices[2] = {};
    int i = 0;
    for (auto it = array.getStartIterator(); it != array.getEndIterator(); ++it)
    {
        values[i] = *it;
        indices[i] = it.getIndex();
        ++i;
    }
    REQUIRE(i == 2);
    REQUIRE(values[0] == 1);
    REQUIRE(values[1] == 3);
    REQUIRE(indices[0] == 0);
    REQUIRE(indices[1] == 2);
}

ARK_TEST_CASE("StableArray reserveFor allows sparse insert", "[collections][stablearray]")
{
    using Ark::Collections::StableArray;

    StableArray<int> array;
    array.reserveFor(15);
    REQUIRE(array.getCapacity() > 15);
    REQUIRE(!array.has(15));

    auto previous = array.insert(15, 99);
    REQUIRE(!previous.hasValue());
    REQUIRE(array.get(15) == 99);
    REQUIRE(array.getNextAppendIndex() == 16);
}
