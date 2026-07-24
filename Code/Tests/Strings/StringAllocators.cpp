#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/StandardAllocator.hpp"
#include "Ark/Memory/TrackingAllocator.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

ARK_TEST_CASE("String move and empty construction with allocators", "[strings][string][allocator]")
{
    using Ark::String;
    using Ark::Memory::StandardAllocator;
    using Ark::Memory::TrackingAllocator;

    SECTION("move constructor rebinds allocator")
    {
        StandardAllocator parent;
        TrackingAllocator tracker{parent};

        {
            String source("hello", tracker);
            REQUIRE(tracker.getBytesAllocated() > 0);

            String moved = Ark::move(source);
            REQUIRE(moved == "hello");
        }

        REQUIRE(tracker.getBytesAllocated() == 0);
    }

    SECTION("move assignment with same allocator steals buffer")
    {
        StandardAllocator parent;
        TrackingAllocator tracker{parent};

        {
            String source("abcdef", tracker);
            String dest("", tracker);
            dest = Ark::move(source);
            REQUIRE(dest == "abcdef");
        }

        REQUIRE(tracker.getBytesAllocated() == 0);
    }

    SECTION("empty capacity constructor is clearable")
    {
        String value(Ark::usize{0});
        REQUIRE(value.isEmpty());
        value.clear();
        REQUIRE(value.isEmpty());
        REQUIRE(value.asPointer()[0] == '\0');
    }

    SECTION("empty slice constructor is clearable")
    {
        Ark::Collections::Slice<char> empty;
        String value(empty);
        REQUIRE(value.isEmpty());
        value.clear();
        REQUIRE(value.asPointer()[0] == '\0');
    }

    SECTION("shrinkToFit preserves NUL terminator")
    {
        String value;
        value.reserve(64);
        value.append(Ark::StringSlice("hi"));
        value.shrinkToFit();
        REQUIRE(value == "hi");
        REQUIRE(value.asPointer()[value.getLength()] == '\0');
    }
}
