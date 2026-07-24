#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/SinglyLinkedList.hpp"

ARK_TEST_CASE("SinglyLinkedList append prepend and ends", "[collections][singlylinkedlist]")
{
    using Ark::Collections::SinglyLinkedList;

    SinglyLinkedList<int> list;
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

    auto last = list.removeLast();
    REQUIRE(last.hasValue());
    REQUIRE(last.getValue() == 3);
    REQUIRE(list.getLast() == 2);
}

ARK_TEST_CASE("SinglyLinkedList iteration and remove", "[collections][singlylinkedlist]")
{
    using Ark::Collections::SinglyLinkedList;

    SinglyLinkedList<int> list{1, 2, 3, 4};
    int expected = 1;
    for (int value : list)
    {
        REQUIRE(value == expected);
        ++expected;
    }

    auto it = list.getStartIterator();
    ++it;
    it = list.remove(it);
    REQUIRE(*it == 3);
    REQUIRE(list.getCount() == 3);
    REQUIRE(list.getFirst() == 1);
    REQUIRE(list.getLast() == 4);
}

ARK_TEST_CASE("SinglyLinkedList appendList and splitAt", "[collections][singlylinkedlist]")
{
    using Ark::Collections::SinglyLinkedList;

    SinglyLinkedList<int> a{1, 2};
    SinglyLinkedList<int> b{3, 4};
    a.appendList(b);

    REQUIRE(a.getCount() == 4);
    REQUIRE(b.isEmpty());

    SinglyLinkedList<int> rest = a.splitAt(2);
    REQUIRE(a.getCount() == 2);
    REQUIRE(rest.getCount() == 2);
    REQUIRE(a.getLast() == 2);
    REQUIRE(rest.getFirst() == 3);
}
