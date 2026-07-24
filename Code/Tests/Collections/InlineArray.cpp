#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/InlineArray.hpp"
#include "Ark/Collections/InlineSlice.hpp"
#include "Ark/Collections/Slice.hpp"

ARK_TEST_CASE("InlineArray", "[collections][inlinearray]")
{
    using Ark::Collections::InlineArray;

    SECTION("construction and assignment")
    {
        InlineArray<int, 4> values{1, 2};
        REQUIRE(values.getCount() == 4);
        REQUIRE(values.getCapacity() == 4);
        REQUIRE(values.getByteCount() == 4 * sizeof(int));
        REQUIRE(values[0] == 1);
        REQUIRE(values[1] == 2);
        REQUIRE(values[2] == 0);
        REQUIRE(values[3] == 0);

        values = {9, 8};
        REQUIRE(values[0] == 9);
        REQUIRE(values[1] == 8);
        REQUIRE(values[2] == 0);
        REQUIRE(values[3] == 0);
    }

    SECTION("access helpers")
    {
        InlineArray<int, 4> values{3, 4, 5, 6};

        REQUIRE(values.getFirst() == 3);
        REQUIRE(values.getLast() == 6);
        REQUIRE(values.get(2) == 5);
        REQUIRE(values[1] == 4);
    }

    SECTION("fill reverse and rotate")
    {
        InlineArray<int, 4> values{1, 2, 3, 4};

        auto reversed = values.reversed();
        REQUIRE(reversed == InlineArray<int, 4>{4, 3, 2, 1});

        auto left = values.rotateLeft(1);
        REQUIRE(left == InlineArray<int, 4>{2, 3, 4, 1});

        auto right = values.rotateRight(1);
        REQUIRE(right == InlineArray<int, 4>{4, 1, 2, 3});

        values.reverse();
        REQUIRE(values == InlineArray<int, 4>{4, 3, 2, 1});

        values.fill(7);
        REQUIRE(values.contains(7));
        REQUIRE(values.getFirst() == 7);
        REQUIRE(values.getLast() == 7);
    }

    SECTION("split and swap")
    {
        InlineArray<int, 4> values{10, 20, 30, 40};
        auto halves = values.splitAt(2);

        REQUIRE(halves.first[0] == 10);
        REQUIRE(halves.first[1] == 20);
        REQUIRE(halves.second[0] == 30);
        REQUIRE(halves.second[1] == 40);

        InlineArray<int, 4> other{1, 2, 3, 4};
        values.swap(other);

        REQUIRE(values == InlineArray<int, 4>{1, 2, 3, 4});
        REQUIRE(other == InlineArray<int, 4>{10, 20, 30, 40});
    }

    SECTION("equality operators")
    {
        InlineArray<int, 4> a{1, 2, 3, 4};
        InlineArray<int, 4> b{1, 2, 3, 4};
        InlineArray<int, 4> c{1, 2, 9, 4};

        REQUIRE(a == b);
        REQUIRE(a != c);
    }
}

ARK_TEST_CASE("InlineSlice", "[collections][slice][inlineslice]")
{
    using Ark::Collections::InlineArray;
    using Ark::Collections::InlineSlice;
    using Ark::Collections::Slice;

    SECTION("construction and element access")
    {
        int raw[3] = {7, 8, 9};
        InlineSlice<int, 3> slice(&raw[0]);

        REQUIRE(slice.getCount() == 3);
        REQUIRE(slice.getByteCount() == 3 * sizeof(int));
        REQUIRE(slice.getFirst() == 7);
        REQUIRE(slice.getLast() == 9);
        REQUIRE(slice.get(1) == 8);
        REQUIRE(slice.contains(8));
        REQUIRE(slice[2] == 9);
    }

    SECTION("slice and static slice view interop")
    {
        InlineArray<int, 3> values{7, 8, 9};
        Slice<int const> slice(values.asPointer(), values.getCount());
        InlineSlice<int, 3> inlineSlice(values.asPointer());

        REQUIRE(slice.getCount() == 3);
        REQUIRE(slice.getFirst() == 7);
        REQUIRE(slice.getLast() == 9);
        REQUIRE(inlineSlice.contains(8));
        REQUIRE(inlineSlice.get(1) == 8);

        values[1] = 42;
        REQUIRE(slice.get(1) == 42);
        REQUIRE(inlineSlice.get(1) == 42);
    }
}
