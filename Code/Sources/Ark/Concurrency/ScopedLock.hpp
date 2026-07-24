#pragma once

#include "Ark/Concurrency/Locks.hpp"
#include "Ark/Concurrency/SpinLock.hpp"
#include "Ark/Core/Macros.hpp"

namespace Ark::Concurrency
{
    /// Scoped multi-lock that acquires distinct lock objects in deterministic address order.
    template <typename... MutexTypes>
    struct ScopedLock final
    {
        ARK_NOT_COPYABLE(ScopedLock);
        ARK_NOT_MOVABLE(ScopedLock);

        static_assert(sizeof...(MutexTypes) > 0, "ScopedLock requires at least one mutex");

        Ark::Tuple<MutexTypes&...> mutexes;
        Collections::InlineArray<Internal::LockEntry, sizeof...(MutexTypes)> entries;

    public:
        explicit ScopedLock(MutexTypes&... ms)
            : mutexes(ms...)
            , entries(Internal::buildLockEntries(mutexes))
        {
            Internal::sortAndValidateLockEntries(entries);
            Internal::lockLockEntries(entries);
        }

        ~ScopedLock()
        {
            Internal::unlockLockEntriesReverse(entries);
        }
    };

    /// Creates a scoped multi-lock for the provided distinct lock objects.
    template <typename... MutexTypes>
    ScopedLock<MutexTypes...> makeScopedLock(MutexTypes&... mutexes)
    {
        return ScopedLock<MutexTypes...>(mutexes...);
    }
}
