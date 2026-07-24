#include "Ark/Core/Defer.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Defer scope-exit", "[core][defer]")
{
    SECTION("Defer executes lambda on scope exit")
    {
        int counter = 0;
        {
            Ark::Defer d{Ark::Function<void()>([&]()
                                               {
                                                   counter = 42;
                                               })};
            REQUIRE(counter == 0);
        }
        REQUIRE(counter == 42);
    }

    SECTION("ARK_DEFER macro executes on scope exit")
    {
        int flag = 0;
        {
            ARK_DEFER([&]
                      {
                          flag = 7;
                      });
            REQUIRE(flag == 0);
        }
        REQUIRE(flag == 7);
    }
}
