#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/CircularBuffer.hpp"

ARK_TEST_CASE("CircularBuffer wraparound ends", "[collections][circularbuffer]")
{
    using Ark::Collections::CircularBuffer;

    auto buffer = CircularBuffer<int>::withCapacity(3);
    REQUIRE(buffer.getCapacity() == 3);
    REQUIRE(buffer.isEmpty());

    buffer.append(1);
    buffer.append(2);
    buffer.append(3);
    REQUIRE(buffer.isFull());
    REQUIRE(!buffer.tryAppend(4));

    REQUIRE(buffer.removeFirst().getValue() == 1);
    REQUIRE(buffer.tryAppend(4));
    REQUIRE(buffer.getFirst() == 2);
    REQUIRE(buffer.getLast() == 4);

    REQUIRE(buffer.removeLast().getValue() == 4);
    buffer.prepend(0);
    REQUIRE(buffer.getFirst() == 0);
    REQUIRE(buffer.getLast() == 3);
    REQUIRE(buffer.getCount() == 3);
}

ARK_TEST_CASE("CircularBuffer iteration order", "[collections][circularbuffer]")
{
    using Ark::Collections::CircularBuffer;

    auto buffer = CircularBuffer<int>::withCapacity(4);
    buffer.append(1);
    buffer.append(2);
    buffer.append(3);
    (void)buffer.removeFirst();
    buffer.append(4);

    int expected[] = {2, 3, 4};
    int i = 0;
    for (int value : buffer)
    {
        REQUIRE(value == expected[i++]);
    }
    REQUIRE(i == 3);
}
