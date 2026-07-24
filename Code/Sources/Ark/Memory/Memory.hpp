#pragma once

#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"
#include "Ark/Memory/SharedPointer.hpp"
#include "Ark/Memory/StandardAllocator.hpp"
#include "Ark/Memory/UniquePointer.hpp"

namespace Ark::Memory
{
    Allocator& getDefaultAllocator();

    /// Set the process-wide default allocator used by containers
    /// @note Not thread-safe if called concurrently with allocations; set during startup
    void setDefaultAllocator(Allocator& allocator);
}
