#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/Arena.hpp"
#include "Ark/Memory/Memory.hpp"

// Declare arenas
ARK_ARENA_DECLARE(TestRoot);
ARK_ARENA_DECLARE(TestChild);

// Define arenas bound to default allocator
ARK_ARENA_DEFINE(TestRoot, Ark::Memory::getDefaultAllocator)
ARK_ARENA_DEFINE_PARENT(TestChild, Ark::Memory::getDefaultAllocator, TestRoot)

ARK_TEST_CASE("Arena", "[arena]")
{
    SECTION("declaration and basic allocation")
    {
        void* p = Ark::Memory::arenaAllocate(ARK_ARENA_GET(TestRoot), 64);
        REQUIRE(p != nullptr);
        Ark::Memory::getDefaultAllocator().deallocate(p);
    }

    SECTION("parented arenas share allocator")
    {
        REQUIRE(&ARK_ARENA_GET(TestChild) != &ARK_ARENA_GET(TestRoot));
        REQUIRE(ARK_ARENA_GET(TestChild).parent == &ARK_ARENA_GET(TestRoot));

        Ark::Memory::Allocator& a0 = ARK_ARENA_GET(TestRoot).allocator();
        Ark::Memory::Allocator& a1 = ARK_ARENA_GET(TestChild).allocator();
        REQUIRE(&a0 == &a1);
    }
}
