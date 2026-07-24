#include "Ark/Testing/Test.hpp"

#include "Ark/Display/Types.hpp"
#include "Ark/Display/Window.hpp"

ARK_TEST_CASE("Display types expose WindowId and Orientation", "[display][types]")
{
    Ark::Display::WindowId id = 0;
    REQUIRE(id == 0);
    REQUIRE(static_cast<Ark::uint32>(Ark::Display::Orientation::Landscape) == 1);
    REQUIRE(static_cast<Ark::uint32>(Ark::Windowing::DisplayOrientation::Landscape) == 1);
}

ARK_TEST_CASE("WindowCreateInfo accepts event dispatcher pointer", "[display][window]")
{
    Ark::Display::WindowCreateInfo info{};
    REQUIRE(info.eventDispatcher == nullptr);
    REQUIRE(info.width == 1280);
}
