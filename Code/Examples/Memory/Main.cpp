#include "Ark/Memory/Functions.hpp"
#include "Ark/Memory/StandardAllocator.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    Ark::Memory::StandardAllocator allocator;

    // Allocate and zero a small buffer
    constexpr Ark::usize count = 8;
    Ark::uint32* values = Ark::Memory::allocateZeroed<Ark::uint32>(allocator, count);
    if (values == nullptr)
    {
        Ark::System::Console::printlnf("Allocation failed");
        return false;
    }

    for (Ark::usize i = 0; i < count; ++i)
    {
        values[i] = static_cast<Ark::uint32>(i * i);
    }

    // Grow the allocation and preserve data
    Ark::uint32* grown = Ark::Memory::reallocate<Ark::uint32>(allocator, values, count, count * 2);
    if (grown == nullptr)
    {
        allocator.deallocate(values);
        Ark::System::Console::printlnf("Reallocate failed");
        return false;
    }

    for (Ark::usize i = 0; i < count; ++i)
    {
        Ark::System::Console::printlnf("v[%zu] = %u", static_cast<size_t>(i), static_cast<unsigned>(grown[i]));
    }

    allocator.deallocate(grown);
    return true;
}
