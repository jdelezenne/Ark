#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Deque.hpp"

ARK_TEST_CASE("Deque append prepend and remove ends", "[collections][deque]")
{
    using Ark::Collections::Deque;

    Deque<int> deque;
    deque.append(2);
    deque.prepend(1);
    deque.append(3);

    REQUIRE(deque.getCount() == 3);
    REQUIRE(deque.getFirst() == 1);
    REQUIRE(deque.getLast() == 3);

    REQUIRE(deque.removeFirst().getValue() == 1);
    REQUIRE(deque.removeLast().getValue() == 3);
    REQUIRE(deque.getFirst() == 2);
}

ARK_TEST_CASE("Deque grows past initial capacity", "[collections][deque]")
{
    using Ark::Collections::Deque;

    auto deque = Deque<int>::withCapacity(2);
    deque.append(1);
    deque.append(2);
    deque.append(3);
    deque.prepend(0);

    REQUIRE(deque.getCount() == 4);
    REQUIRE(deque.getCapacity() >= 4);
    REQUIRE(deque.getFirst() == 0);
    REQUIRE(deque.getLast() == 3);
}
