#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Algorithms.hpp"
#include "Ark/Collections/Array.hpp"
#include "Ark/Collections/Slice.hpp"

#include <initializer_list>
#include <type_traits>

static_assert(!std::is_constructible_v<Ark::Collections::Slice<int>, std::initializer_list<int>>);
static_assert(!std::is_assignable_v<Ark::Collections::Slice<int>&, std::initializer_list<int>>);

ARK_TEST_CASE("Array", "[collections][array][algorithms]")
{
    using Ark::Collections::Array;
    using Ark::Collections::copy;
    using Ark::Collections::distance;
    using Ark::Collections::equal;
    using Ark::Collections::find;
    using Ark::Collections::reverse;
    using Ark::Collections::Slice;

    SECTION("construction mutation and removal")
    {
        Array<int> values;

        REQUIRE(values.isEmpty());

        values.append(1);
        values.append(2);
        values.append(3);

        REQUIRE(values.getCount() == 3);
        REQUIRE(values.getFirst() == 1);
        REQUIRE(values.getLast() == 3);
        REQUIRE(values.contains(2));
    }

    SECTION("sized construction value-initializes elements")
    {
        Array<int> values(3);

        REQUIRE(values.getCount() == 3);
        REQUIRE(values[0] == 0);
        REQUIRE(values[1] == 0);
        REQUIRE(values[2] == 0);
    }

    SECTION("insert split and remove")
    {
        Array<int> values{1, 2, 3, 2};

        values.insert(1, 9);
        REQUIRE(values == Array<int>{1, 9, 2, 3, 2});

        REQUIRE(values.remove(9));
        REQUIRE(values == Array<int>{1, 2, 3, 2});

        REQUIRE(values.removeAll(2) == 2);
        REQUIRE(values == Array<int>{1, 3});

        auto parts = values.splitAt(1);
        REQUIRE(parts.first == Array<int>{1});
        REQUIRE(parts.second == Array<int>{3});
    }

    SECTION("reserve resize and slice view")
    {
        Array<int> values;

        values.reserve(4);
        values.resize(3);

        REQUIRE(values.getCount() == 3);
        REQUIRE(values.getCapacity() >= 4);

        values[0] = 7;
        values[1] = 8;
        values[2] = 9;

        Slice<int const> view = values.asSlice();
        REQUIRE(view.getCount() == 3);
        REQUIRE(view.getFirst() == 7);
        REQUIRE(view.getLast() == 9);
    }

    SECTION("append range variants preserve content")
    {
        Array<int> values{1};

        values.append({2, 3});
        values.append(Slice<int>(values.asMutablePointer(), 1));

        REQUIRE(values == Array<int>{1, 2, 3, 1});
    }

    SECTION("append aliased slice remains valid when growth occurs")
    {
        Array<int> values{1};
        Slice<int> alias(values.asMutablePointer(), values.getCount());

        values.append(alias);

        REQUIRE(values == Array<int>{1, 1});
    }

    SECTION("algorithms and iterator helpers")
    {
        Array<int> source{1, 2, 3, 4};
        Array<int> destination;
        destination.resize(4);

        REQUIRE(distance(source.getStartIterator(), source.getEndIterator()) == 4);
        copy(source.getStartIterator(), source.getEndIterator(), destination.getStartIterator());

        REQUIRE(destination == source);
        REQUIRE(equal(source.getStartIterator(), source.getEndIterator(), destination.getStartIterator(), destination.getEndIterator()));
    }

    SECTION("algorithms reverse and find")
    {
        Array<int> values{4, 3, 2, 1};

        REQUIRE(find(values.getStartIterator(), values.getEndIterator(), 2) != values.getEndIterator());

        reverse(values.getStartIterator(), values.getEndIterator());
        REQUIRE(values == Array<int>{1, 2, 3, 4});
    }
}
