#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/LinkedList.hpp"

ARK_TEST_CASE("LinkedList append prepend and ends", "[collections][linkedlist]")
{
    using Ark::Collections::LinkedList;

    LinkedList<int> list;
    REQUIRE(list.isEmpty());

    list.append(2);
    list.prepend(1);
    list.append(3);

    REQUIRE(list.getCount() == 3);
    REQUIRE(list.getFirst() == 1);
    REQUIRE(list.getLast() == 3);

    auto first = list.removeFirst();
    REQUIRE(first.hasValue());
    REQUIRE(first.getValue() == 1);
    REQUIRE(list.getFirst() == 2);

    auto last = list.removeLast();
    REQUIRE(last.hasValue());
    REQUIRE(last.getValue() == 3);
    REQUIRE(list.getLast() == 2);
}

ARK_TEST_CASE("LinkedList iteration and remove", "[collections][linkedlist]")
{
    using Ark::Collections::LinkedList;

    LinkedList<int> list{1, 2, 3, 4};
    int sum = 0;
    for (int value : list)
    {
        sum += value;
    }
    REQUIRE(sum == 10);

    auto it = list.getStartIterator();
    ++it;
    it = list.remove(it);
    REQUIRE(*it == 3);
    REQUIRE(list.getCount() == 3);
    REQUIRE(list.getFirst() == 1);
    REQUIRE(list.getLast() == 4);
}

ARK_TEST_CASE("LinkedList appendList and splitAt", "[collections][linkedlist]")
{
    using Ark::Collections::LinkedList;

    LinkedList<int> a{1, 2};
    LinkedList<int> b{3, 4};
    a.appendList(b);

    REQUIRE(a.getCount() == 4);
    REQUIRE(b.isEmpty());
    REQUIRE(a.getFirst() == 1);
    REQUIRE(a.getLast() == 4);

    LinkedList<int> rest = a.splitAt(2);
    REQUIRE(a.getCount() == 2);
    REQUIRE(rest.getCount() == 2);
    REQUIRE(a.getFirst() == 1);
    REQUIRE(a.getLast() == 2);
    REQUIRE(rest.getFirst() == 3);
    REQUIRE(rest.getLast() == 4);
}

ARK_TEST_CASE("LinkedList copy and move", "[collections][linkedlist]")
{
    using Ark::Collections::LinkedList;

    LinkedList<int> original{10, 20};
    LinkedList<int> copied = original;
    REQUIRE(copied.getCount() == 2);
    REQUIRE(copied.getFirst() == 10);

    LinkedList<int> moved = Ark::move(original);
    REQUIRE(moved.getCount() == 2);
    REQUIRE(original.isEmpty());
}
