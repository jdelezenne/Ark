#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    /// Arena descriptor bound to an allocator provider.
    struct Arena final
    {
        /// Function pointer returning the allocator backing this arena.
        using AllocatorProvider = Allocator& (*)();

        /// Arena name used for diagnostics and categorization.
        const char* name{nullptr};

        /// Callback returning the allocator bound to this arena.
        AllocatorProvider acquireAllocator{nullptr};

        /// Optional parent arena.
        Arena const* parent{nullptr};

        /// Creates an arena with no parent.
        /// @param name Arena name.
        /// @param provider Allocator provider callback.
        constexpr Arena(const char* name, AllocatorProvider provider)
            : name{name}
            , acquireAllocator{provider}
        {
        }

        /// Creates an arena with an explicit parent.
        /// @param name Arena name.
        /// @param provider Allocator provider callback.
        /// @param parent Parent arena.
        constexpr Arena(const char* name, AllocatorProvider provider, Arena const* parent)
            : name{name}
            , acquireAllocator{provider}
            , parent{parent}
        {
        }

        /// Returns the allocator bound to this arena.
        /// @return Arena allocator reference.
        Allocator& allocator() const
        {
            return acquireAllocator();
        }
    };
}

// Arena macros (Unreal-style, mirroring logging category macros)
/// Declares a named global arena symbol.
#define ARK_ARENA_DECLARE(ArenaName) \
    extern const ::Ark::Memory::Arena ArkArena##ArenaName;

/// Defines a named global arena symbol with no parent.
#define ARK_ARENA_DEFINE(ArenaName, AllocatorProviderFn) \
    const ::Ark::Memory::Arena ArkArena##ArenaName{#ArenaName, AllocatorProviderFn};

/// Defines a named global arena symbol with a parent arena.
#define ARK_ARENA_DEFINE_PARENT(ArenaName, AllocatorProviderFn, ParentArenaName) \
    const ::Ark::Memory::Arena ArkArena##ArenaName{#ArenaName, AllocatorProviderFn, &ArkArena##ParentArenaName};

/// Resolves a named arena symbol.
#define ARK_ARENA_GET(ArenaName) ArkArena##ArenaName

// Convenience allocation helpers
namespace Ark::Memory
{
    /// Allocates memory through the arena's allocator.
    /// @param arena Arena to allocate from.
    /// @param size Allocation size in bytes.
    /// @return Allocated memory pointer, or nullptr on failure.
    inline void* arenaAllocate(Arena const& arena, usize size)
    {
        return tryAllocate(arena.allocator(), size);
    }

    /// Allocates aligned memory through the arena's allocator.
    /// @param arena Arena to allocate from.
    /// @param size Allocation size in bytes.
    /// @param alignment Required alignment in bytes.
    /// @return Allocated memory pointer, or nullptr on failure.
    inline void* arenaAllocateAligned(Arena const& arena, usize size, usize alignment)
    {
        return tryAllocateAligned(arena.allocator(), size, alignment);
    }
}
