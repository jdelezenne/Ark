#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/BitSet.hpp"

ARK_TEST_CASE("BitSet set clear toggle and counts", "[collections][bitset]")
{
    using Ark::Collections::BitSet;

    BitSet bits = BitSet::withCount(8);
    REQUIRE(bits.getCount() == 8);
    REQUIRE(bits.none());
    REQUIRE(bits.getSetCount() == 0);

    bits.set(1);
    bits.set(3);
    bits.set(7);
    REQUIRE(bits.has(1));
    REQUIRE(!bits.has(0));
    REQUIRE(bits.getSetCount() == 3);
    REQUIRE(bits.any());
    REQUIRE(!bits.all());

    bits.clear(3);
    REQUIRE(!bits.has(3));
    REQUIRE(bits.getSetCount() == 2);

    bits.toggle(0);
    REQUIRE(bits.has(0));
    bits.toggle(0);
    REQUIRE(!bits.has(0));
}

ARK_TEST_CASE("BitSet resize append and setAll", "[collections][bitset]")
{
    using Ark::Collections::BitSet;

    BitSet bits;
    bits.append(true);
    bits.append(false);
    bits.append(true);
    REQUIRE(bits.getCount() == 3);
    REQUIRE(bits.has(0));
    REQUIRE(!bits.has(1));
    REQUIRE(bits.has(2));

    bits.resize(5, true);
    REQUIRE(bits.getCount() == 5);
    REQUIRE(bits.has(3));
    REQUIRE(bits.has(4));

    bits.setAll();
    REQUIRE(bits.all());
    REQUIRE(bits.getSetCount() == 5);

    bits.clearAll();
    REQUIRE(bits.none());
}

ARK_TEST_CASE("BitSet bitwise operators", "[collections][bitset]")
{
    using Ark::Collections::BitSet;

    BitSet a = BitSet::withCount(4);
    BitSet b = BitSet::withCount(4);
    a.set(0);
    a.set(1);
    b.set(1);
    b.set(2);

    BitSet anded = a & b;
    REQUIRE(anded.has(1));
    REQUIRE(!anded.has(0));
    REQUIRE(!anded.has(2));

    BitSet ored = a | b;
    REQUIRE(ored.has(0));
    REQUIRE(ored.has(1));
    REQUIRE(ored.has(2));

    BitSet xored = a ^ b;
    REQUIRE(xored.has(0));
    REQUIRE(!xored.has(1));
    REQUIRE(xored.has(2));
}

ARK_TEST_CASE("BitSet across word boundary", "[collections][bitset]")
{
    using Ark::Collections::BitSet;

    BitSet bits = BitSet::withCount(70);
    bits.set(0);
    bits.set(63);
    bits.set(64);
    bits.set(69);

    REQUIRE(bits.getSetCount() == 4);
    REQUIRE(bits.has(63));
    REQUIRE(bits.has(64));
    REQUIRE(bits.tryHas(100) == false);

    bits.resize(64);
    REQUIRE(bits.getCount() == 64);
    REQUIRE(bits.has(63));
    REQUIRE(!bits.tryHas(64));
    REQUIRE(bits.getSetCount() == 2);
}
