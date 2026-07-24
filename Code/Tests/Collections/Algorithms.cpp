#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Algorithms.hpp"
#include "Ark/Collections/Array.hpp"

namespace
{
    struct Pair
    {
        int key = 0;
        int id = 0;

        bool operator==(Pair const& other) const
        {
            return key == other.key && id == other.id;
        }
    };

    constexpr bool byKey(Pair const& lhs, Pair const& rhs)
    {
        return lhs.key < rhs.key;
    }
}

ARK_TEST_CASE("Algorithms predicates and search", "[collections][algorithms]")
{
    using Ark::Collections::allOf;
    using Ark::Collections::anyOf;
    using Ark::Collections::Array;
    using Ark::Collections::equal;
    using Ark::Collections::find;
    using Ark::Collections::findIf;
    using Ark::Collections::lexicographicalCompare;

    SECTION("find findIf anyOf allOf")
    {
        Array<int> values{1, 3, 5, 7};

        REQUIRE(find(values.getStartIterator(), values.getEndIterator(), 5) == values.getStartIterator() + 2);
        REQUIRE(find(values.getStartIterator(), values.getEndIterator(), 4) == values.getEndIterator());

        auto even = findIf(values.getStartIterator(), values.getEndIterator(), [](int value) {
            return value % 2 == 0;
        });
        REQUIRE(even == values.getEndIterator());

        auto gtFour = findIf(values.getStartIterator(), values.getEndIterator(), [](int value) {
            return value > 4;
        });
        REQUIRE(gtFour != values.getEndIterator());
        REQUIRE(*gtFour == 5);

        REQUIRE(anyOf(values.getStartIterator(), values.getEndIterator(), [](int value) {
            return value == 7;
        }));
        REQUIRE(!anyOf(values.getStartIterator(), values.getEndIterator(), [](int value) {
            return value < 0;
        }));

        REQUIRE(allOf(values.getStartIterator(), values.getEndIterator(), [](int value) {
            return value > 0;
        }));
        REQUIRE(!allOf(values.getStartIterator(), values.getEndIterator(), [](int value) {
            return value < 5;
        }));
    }

    SECTION("equal and lexicographicalCompare")
    {
        Array<int> left{1, 2, 3};
        Array<int> right{1, 2, 3};
        Array<int> shorter{1, 2};
        Array<int> greater{1, 2, 4};

        REQUIRE(equal(left.getStartIterator(), left.getEndIterator(), right.getStartIterator(), right.getEndIterator()));
        REQUIRE(!equal(left.getStartIterator(), left.getEndIterator(), shorter.getStartIterator(), shorter.getEndIterator()));

        REQUIRE(lexicographicalCompare(
            shorter.getStartIterator(),
            shorter.getEndIterator(),
            left.getStartIterator(),
            left.getEndIterator(),
            Ark::Collections::Less<>{}));
        REQUIRE(!lexicographicalCompare(
            left.getStartIterator(),
            left.getEndIterator(),
            shorter.getStartIterator(),
            shorter.getEndIterator(),
            Ark::Collections::Less<>{}));
        REQUIRE(lexicographicalCompare(
            left.getStartIterator(),
            left.getEndIterator(),
            greater.getStartIterator(),
            greater.getEndIterator(),
            Ark::Collections::Less<>{}));
    }
}

ARK_TEST_CASE("Algorithms min max clamp and extrema", "[collections][algorithms]")
{
    using Ark::Collections::Array;
    using Ark::Collections::clamp;
    using Ark::Collections::max;
    using Ark::Collections::maxElement;
    using Ark::Collections::min;
    using Ark::Collections::minElement;

    SECTION("min max clamp")
    {
        REQUIRE(min(3, 1) == 1);
        REQUIRE(max(3, 1) == 3);
        REQUIRE(clamp(0, 1, 5) == 1);
        REQUIRE(clamp(3, 1, 5) == 3);
        REQUIRE(clamp(9, 1, 5) == 5);

        auto greater = [](int lhs, int rhs) {
            return lhs > rhs;
        };
        REQUIRE(min(3, 1, greater) == 3);
        REQUIRE(max(3, 1, greater) == 1);
        // With greater-as-less, bounds are [5, 1] in that ordering.
        REQUIRE(clamp(0, 5, 1, greater) == 1);
        REQUIRE(clamp(3, 5, 1, greater) == 3);
        REQUIRE(clamp(9, 5, 1, greater) == 5);
    }

    SECTION("minElement maxElement empty and populated")
    {
        Array<int> empty;
        REQUIRE(minElement(empty.getStartIterator(), empty.getEndIterator()) == empty.getEndIterator());
        REQUIRE(maxElement(empty.getStartIterator(), empty.getEndIterator()) == empty.getEndIterator());

        Array<int> values{4, 1, 7, 1, 3};
        REQUIRE(*minElement(values.getStartIterator(), values.getEndIterator()) == 1);
        REQUIRE(minElement(values.getStartIterator(), values.getEndIterator()) == values.getStartIterator() + 1);
        REQUIRE(*maxElement(values.getStartIterator(), values.getEndIterator()) == 7);
    }
}

ARK_TEST_CASE("Algorithms fill unique reverse isSorted", "[collections][algorithms]")
{
    using Ark::Collections::Array;
    using Ark::Collections::fill;
    using Ark::Collections::isSorted;
    using Ark::Collections::reverse;
    using Ark::Collections::unique;

    SECTION("fill reverse and isSorted")
    {
        Array<int> values{1, 2, 3, 4};
        REQUIRE(isSorted(values.getStartIterator(), values.getEndIterator()));

        reverse(values.getStartIterator(), values.getEndIterator());
        REQUIRE(values == Array<int>{4, 3, 2, 1});
        REQUIRE(!isSorted(values.getStartIterator(), values.getEndIterator()));
        REQUIRE(isSorted(values.getStartIterator(), values.getEndIterator(), [](int lhs, int rhs) {
            return lhs > rhs;
        }));

        fill(values.getStartIterator(), values.getEndIterator(), 9);
        REQUIRE(values == Array<int>{9, 9, 9, 9});
        REQUIRE(isSorted(values.getStartIterator(), values.getEndIterator()));
    }

    SECTION("unique collapses consecutive duplicates")
    {
        Array<int> values{1, 1, 2, 2, 2, 3, 3};
        auto newEnd = unique(values.getStartIterator(), values.getEndIterator());
        values.resize(static_cast<Ark::usize>(newEnd - values.getStartIterator()));
        REQUIRE(values == Array<int>{1, 2, 3});

        Array<int> alreadyUnique{1, 2, 3};
        auto sameEnd = unique(alreadyUnique.getStartIterator(), alreadyUnique.getEndIterator());
        REQUIRE(sameEnd == alreadyUnique.getEndIterator());

        Array<int> empty;
        REQUIRE(unique(empty.getStartIterator(), empty.getEndIterator()) == empty.getEndIterator());
    }
}

ARK_TEST_CASE("Algorithms sort and stableSort", "[collections][algorithms]")
{
    using Ark::Collections::Array;
    using Ark::Collections::isSorted;
    using Ark::Collections::sort;
    using Ark::Collections::stableSort;

    SECTION("sort ascending and custom compare")
    {
        Array<int> values{5, 1, 4, 2, 3};
        sort(values);
        REQUIRE(values == Array<int>{1, 2, 3, 4, 5});
        REQUIRE(isSorted(values.getStartIterator(), values.getEndIterator()));

        sort(values, [](int lhs, int rhs) {
            return lhs > rhs;
        });
        REQUIRE(values == Array<int>{5, 4, 3, 2, 1});
    }

    SECTION("sort handles empty single and already sorted")
    {
        Array<int> empty;
        sort(empty);
        REQUIRE(empty.isEmpty());

        Array<int> single{42};
        sort(single);
        REQUIRE(single == Array<int>{42});

        Array<int> sorted{1, 2, 3};
        sort(sorted);
        REQUIRE(sorted == Array<int>{1, 2, 3});
    }

    SECTION("sort larger range uses quicksort path")
    {
        Array<int> values;
        for (int i = 40; i >= 1; --i)
        {
            values.append(i);
        }

        sort(values);
        REQUIRE(values.getCount() == 40);
        REQUIRE(isSorted(values.getStartIterator(), values.getEndIterator()));
        REQUIRE(values.getFirst() == 1);
        REQUIRE(values.getLast() == 40);
    }

    SECTION("stableSort preserves relative order of equal keys")
    {
        Array<Pair> values{
            Pair{2, 1},
            Pair{1, 1},
            Pair{2, 2},
            Pair{1, 2},
            Pair{2, 3},
        };

        stableSort(values, byKey);

        REQUIRE(values == Array<Pair>{
                              Pair{1, 1},
                              Pair{1, 2},
                              Pair{2, 1},
                              Pair{2, 2},
                              Pair{2, 3},
                          });
    }
}

ARK_TEST_CASE("Algorithms bounds and binarySearch", "[collections][algorithms]")
{
    using Ark::Collections::Array;
    using Ark::Collections::binarySearch;
    using Ark::Collections::lowerBound;
    using Ark::Collections::upperBound;

    Array<int> values{1, 3, 3, 3, 5, 7};

    SECTION("lowerBound and upperBound around duplicates")
    {
        auto lower = lowerBound(values.getStartIterator(), values.getEndIterator(), 3);
        auto upper = upperBound(values.getStartIterator(), values.getEndIterator(), 3);
        REQUIRE(lower == values.getStartIterator() + 1);
        REQUIRE(upper == values.getStartIterator() + 4);
        REQUIRE(static_cast<Ark::usize>(upper - lower) == 3);

        REQUIRE(lowerBound(values.getStartIterator(), values.getEndIterator(), 0) == values.getStartIterator());
        REQUIRE(upperBound(values.getStartIterator(), values.getEndIterator(), 0) == values.getStartIterator());
        REQUIRE(lowerBound(values.getStartIterator(), values.getEndIterator(), 9) == values.getEndIterator());
        REQUIRE(upperBound(values.getStartIterator(), values.getEndIterator(), 9) == values.getEndIterator());
    }

    SECTION("binarySearch finds present and missing values")
    {
        REQUIRE(binarySearch(values.getStartIterator(), values.getEndIterator(), 1));
        REQUIRE(binarySearch(values.getStartIterator(), values.getEndIterator(), 3));
        REQUIRE(binarySearch(values.getStartIterator(), values.getEndIterator(), 7));
        REQUIRE(!binarySearch(values.getStartIterator(), values.getEndIterator(), 0));
        REQUIRE(!binarySearch(values.getStartIterator(), values.getEndIterator(), 4));
        REQUIRE(!binarySearch(values.getStartIterator(), values.getEndIterator(), 8));

        Array<int> empty;
        REQUIRE(!binarySearch(empty.getStartIterator(), empty.getEndIterator(), 1));
    }
}
