#pragma once

#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark::Memory
{
    /// Memory protection flags used by virtual-memory operations.
    enum class Protect : uint32
    {
        NoAccess = 0,
        Read = ARK_BIT(0),
        Write = ARK_BIT(1),
        Execute = ARK_BIT(2),
        ReadWrite = Read | Write,
        ReadExecute = Read | Execute,
        ReadWriteExecute = Read | Write | Execute,
    };

    /// Reserves a virtual-memory region without committing physical pages.
    /// @param size Region size in bytes.
    /// @param alignment Reservation alignment.
    /// @return Base address of reserved region, or nullptr on failure.
    void* reserve(usize size, usize alignment = 4096);

    /// Commits pages within a reserved virtual-memory region.
    /// @param base Base address inside a reserved region.
    /// @param size Size in bytes to commit.
    /// @param protection Initial protection flags.
    /// @return True on success, false otherwise.
    bool commit(void* base, usize size, Protect protection = Protect::ReadWrite);

    /// Decommits pages from a reserved region.
    /// @param base Base address of the committed range.
    /// @param size Size in bytes to decommit.
    /// @return True on success, false otherwise.
    bool decommit(void* base, usize size);

    /// Releases a previously reserved virtual-memory region.
    /// @param base Base address returned by reserve().
    /// @param size Reserved size in bytes.
    /// @return True on success, false otherwise.
    bool release(void* base, usize size);

    /// Changes memory protection for a committed range.
    /// @param base Base address of the range.
    /// @param size Size in bytes.
    /// @param protection New protection flags.
    /// @return True on success, false otherwise.
    bool protect(void* base, usize size, Protect protection);
}
