#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Queue.hpp"

ARK_TEST_CASE("Queue enqueue dequeue FIFO", "[collections][queue]")
{
    using Ark::Collections::Queue;

    Queue<int> queue;
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    REQUIRE(queue.getCount() == 3);
    REQUIRE(queue.getFirst() == 1);

    REQUIRE(queue.dequeue().getValue() == 1);
    REQUIRE(queue.dequeue().getValue() == 2);
    REQUIRE(queue.dequeue().getValue() == 3);
    REQUIRE(queue.isEmpty());
    REQUIRE(!queue.dequeue().hasValue());
}
