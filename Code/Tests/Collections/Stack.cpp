#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Stack.hpp"

ARK_TEST_CASE("Stack push pop LIFO", "[collections][stack]")
{
    using Ark::Collections::Stack;

    Stack<int> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);

    REQUIRE(stack.getCount() == 3);
    REQUIRE(stack.getTop() == 3);

    REQUIRE(stack.pop().getValue() == 3);
    REQUIRE(stack.pop().getValue() == 2);
    REQUIRE(stack.pop().getValue() == 1);
    REQUIRE(stack.isEmpty());
    REQUIRE(!stack.pop().hasValue());
}
