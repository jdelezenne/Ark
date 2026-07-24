#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/LinearAllocator.hpp"
#include "Ark/Memory/StandardAllocator.hpp"
#include "Ark/Memory/TrackingAllocator.hpp"

ARK_TEST_CASE("TrackingAllocator tracks live bytes", "[memory][tracking]")
{
    Ark::Memory::StandardAllocator parent;
    Ark::Memory::TrackingAllocator tracker{parent};

    SECTION("bytesAllocated increments and decrements")
    {
        void* a = tracker.allocate(64);
        REQUIRE(a != nullptr);
        REQUIRE(tracker.getBytesAllocated() == 64);
        REQUIRE(tracker.getPeakBytesAllocated() == 64);
        REQUIRE(tracker.getAllocationCount() == 1);

        void* b = tracker.allocate(32);
        REQUIRE(b != nullptr);
        REQUIRE(tracker.getBytesAllocated() == 96);
        REQUIRE(tracker.getPeakBytesAllocated() == 96);
        REQUIRE(tracker.getAllocationCount() == 2);

        tracker.deallocate(b);
        REQUIRE(tracker.getBytesAllocated() == 64);
        REQUIRE(tracker.getPeakBytesAllocated() == 96);

        tracker.deallocate(a);
        REQUIRE(tracker.getBytesAllocated() == 0);
    }

    SECTION("aligned paths are tracked")
    {
        void* p = tracker.allocateAligned(128, 64);
        REQUIRE(p != nullptr);
        REQUIRE(tracker.getBytesAllocated() == 128);
        REQUIRE(tracker.getPeakBytesAllocated() == 128);

        tracker.deallocateAligned(p);
        REQUIRE(tracker.getBytesAllocated() == 0);
    }

    SECTION("reset clears counters")
    {
        void* p = tracker.allocate(16);
        REQUIRE(p != nullptr);
        REQUIRE(tracker.getBytesAllocated() == 16);

        tracker.reset();
        REQUIRE(tracker.getBytesAllocated() == 0);
        REQUIRE(tracker.getPeakBytesAllocated() == 0);
        REQUIRE(tracker.getAllocationCount() == 0);
    }
}

ARK_TEST_CASE("TrackingAllocator works over LinearAllocator parent", "[memory][tracking]")
{
    alignas(64) Ark::byte buffer[4096];
    Ark::Memory::LinearAllocator arena{buffer, sizeof(buffer), 64};
    Ark::Memory::TrackingAllocator tracker{arena};

    void* a = tracker.allocate(64);
    void* b = tracker.allocateAligned(128, 64);
    REQUIRE(a != nullptr);
    REQUIRE(b != nullptr);
    REQUIRE(tracker.getBytesAllocated() == 192);

    tracker.reset();
    REQUIRE(tracker.getBytesAllocated() == 0);

    void* c = tracker.allocate(32);
    REQUIRE(c != nullptr);
    tracker.deallocate(c);
}
