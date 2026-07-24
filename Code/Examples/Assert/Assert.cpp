#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/System/EntryPoint.hpp"

static int divide(int a, int b)
{
    ARK_ASSERT_MSG(b != 0, "divide() denominator must be non-zero");
    return a / b;
}

bool arkMain(Ark::Collections::Array<Ark::String> const& arguments)
{
    divide(1, 0);

    return true;
}
