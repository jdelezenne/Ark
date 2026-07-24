#include "Ark/Memory/Arena.hpp"
#include "Ark/Memory/Memory.hpp"
#include "Ark/System/EntryPoint.hpp"

// Declare and define a demo arena using the default allocator
ARK_ARENA_DECLARE(Demo);
ARK_ARENA_DEFINE(Demo, Ark::Memory::getDefaultAllocator)

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    void* p = Ark::Memory::arenaAllocate(ARK_ARENA_GET(Demo), 128);
    if (p == nullptr)
    {
        return false;
    }
    ARK_ARENA_GET(Demo).allocator().deallocate(p);
    return true;
}
