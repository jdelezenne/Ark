#pragma once

#include "Ark/Collections/InlineArray.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Tuple.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"

namespace Ark::Concurrency
{
    namespace Internal
    {
        struct LockEntry final
        {
            void* address;
            uintptr sortKey;
            usize originalIndex;
            void (*lockFunction)(void*);
            bool (*tryLockFunction)(void*);
            void (*unlockFunction)(void*);
        };

        template <typename LockType>
        LockEntry makeLockEntry(LockType& lock, usize originalIndex)
        {
            return LockEntry{
                static_cast<void*>(Ark::addressOf(lock)),
                reinterpret_cast<uintptr>(Ark::addressOf(lock)),
                originalIndex,
                [](void* address)
                {
                    static_cast<LockType*>(address)->lock();
                },
                [](void* address)
                {
                    return static_cast<LockType*>(address)->tryLock();
                },
                [](void* address)
                {
                    static_cast<LockType*>(address)->unlock();
                }};
        }

        template <usize Index, usize Count, typename TupleType>
        void populateLockEntries(TupleType& locks, Collections::InlineArray<LockEntry, Count>& entries)
        {
            if constexpr (Index < Count)
            {
                entries[Index] = makeLockEntry(Ark::get<Index>(locks), Index);
                populateLockEntries<Index + 1>(locks, entries);
            }
        }

        template <typename... LockTypes>
        Collections::InlineArray<LockEntry, sizeof...(LockTypes)> buildLockEntries(Ark::Tuple<LockTypes&...>& locks)
        {
            Collections::InlineArray<LockEntry, sizeof...(LockTypes)> entries;
            populateLockEntries<0>(locks, entries);
            return entries;
        }

        template <usize Count>
        void sortAndValidateLockEntries(Collections::InlineArray<LockEntry, Count>& entries)
        {
            Collections::sort(entries, [](LockEntry const& left, LockEntry const& right)
                              {
                                  return left.sortKey < right.sortKey;
                              });

            for (usize i = 1; i < Count; ++i)
            {
                ARK_ASSERT(entries[i - 1].address != entries[i].address);
            }
        }

        template <usize Count>
        void lockLockEntries(Collections::InlineArray<LockEntry, Count> const& entries)
        {
            for (LockEntry const& entry : entries)
            {
                entry.lockFunction(entry.address);
            }
        }

        template <usize Count>
        void unlockLockEntriesReverse(Collections::InlineArray<LockEntry, Count> const& entries)
        {
            for (usize i = Count; i > 0; --i)
            {
                LockEntry const& entry = entries[i - 1];
                entry.unlockFunction(entry.address);
            }
        }
    }

    /// Defer acquisition of a lock until `lock()` or `tryLock()` is called.
    struct DeferLockTag final
    {
    };

    /// Attempt to acquire a lock without blocking during construction.
    struct TryToLockTag final
    {
    };

    /// Adopt ownership of a lock that has already been acquired by the caller.
    struct AdoptLockTag final
    {
    };

    inline constexpr DeferLockTag deferLock{};
    inline constexpr TryToLockTag tryToLock{};
    inline constexpr AdoptLockTag adoptLock{};

    /// Lock guard for automatic unlock
    template <typename LockType>
    struct LockGuard final
    {
        ARK_NOT_COPYABLE(LockGuard);
        ARK_NOT_MOVABLE(LockGuard);

    private:
        LockType& lockRef;

    public:
        explicit LockGuard(LockType& lock)
            : lockRef(lock)
        {
            lockRef.lock();
        }

        LockGuard(LockType& lock, AdoptLockTag)
            : lockRef(lock)
        {
        }

        ~LockGuard()
        {
            lockRef.unlock();
        }
    };

    /// Unique lock with deferred locking support
    template <typename LockType>
    struct UniqueLock final
    {
    private:
        LockType* lockPtr{nullptr};
        bool isLocked{false};

    public:
        UniqueLock() = default;

        explicit UniqueLock(LockType& lock)
            : lockPtr(&lock)
        {
            lockPtr->lock();
            isLocked = true;
        }

        UniqueLock(LockType& lock, DeferLockTag)
            : lockPtr(&lock)
        {
        }

        UniqueLock(LockType& lock, TryToLockTag)
            : lockPtr(&lock)
            , isLocked(lock.tryLock())
        {
        }

        UniqueLock(LockType& lock, AdoptLockTag)
            : lockPtr(&lock)
            , isLocked(true)
        {
        }

        UniqueLock(UniqueLock&& other)
            : lockPtr(other.lockPtr)
            , isLocked(other.isLocked)
        {
            other.lockPtr = nullptr;
            other.isLocked = false;
        }

        UniqueLock& operator=(UniqueLock&& other)
        {
            if (this != &other)
            {
                if (isLocked)
                {
                    ARK_ASSERT(lockPtr != nullptr);
                    lockPtr->unlock();
                }

                lockPtr = other.lockPtr;
                isLocked = other.isLocked;
                other.lockPtr = nullptr;
                other.isLocked = false;
            }
            return *this;
        }

        ~UniqueLock()
        {
            if (isLocked)
            {
                ARK_ASSERT(lockPtr != nullptr);
                lockPtr->unlock();
            }
        }

        UniqueLock(UniqueLock const&) = delete;
        UniqueLock& operator=(UniqueLock const&) = delete;

        void swap(UniqueLock& other)
        {
            Ark::swap(lockPtr, other.lockPtr);
            Ark::swap(isLocked, other.isLocked);
        }

        void lock()
        {
            ARK_ASSERT(lockPtr != nullptr);
            ARK_ASSERT(!isLocked);
            lockPtr->lock();
            isLocked = true;
        }

        bool tryLock()
        {
            ARK_ASSERT(lockPtr != nullptr);
            ARK_ASSERT(!isLocked);
            isLocked = lockPtr->tryLock();
            return isLocked;
        }

        void unlock()
        {
            ARK_ASSERT(lockPtr != nullptr);
            ARK_ASSERT(isLocked);
            lockPtr->unlock();
            isLocked = false;
        }

        LockType* release()
        {
            LockType* released = lockPtr;
            lockPtr = nullptr;
            isLocked = false;
            return released;
        }

        bool ownsLock() const
        {
            return isLocked;
        }

        LockType* mutex() const
        {
            return lockPtr;
        }

        explicit operator bool() const
        {
            return ownsLock();
        }
    };

    /// Shared lock for reader-writer scenarios
    template <typename SharedLockType>
    struct SharedLock final
    {
        ARK_NOT_COPYABLE(SharedLock);

    private:
        SharedLockType* lockPtr{nullptr};
        bool isLocked{false};

    public:
        SharedLock() = default;

        explicit SharedLock(SharedLockType& lock)
            : lockPtr(&lock)
        {
            lockPtr->lockShared();
            isLocked = true;
        }

        SharedLock(SharedLockType& lock, DeferLockTag)
            : lockPtr(&lock)
        {
        }

        SharedLock(SharedLockType& lock, TryToLockTag)
            : lockPtr(&lock)
            , isLocked(lock.tryLockShared())
        {
        }

        SharedLock(SharedLockType& lock, AdoptLockTag)
            : lockPtr(&lock)
            , isLocked(true)
        {
        }

        SharedLock(SharedLock&& other)
            : lockPtr(other.lockPtr)
            , isLocked(other.isLocked)
        {
            other.lockPtr = nullptr;
            other.isLocked = false;
        }

        SharedLock& operator=(SharedLock&& other)
        {
            if (this != &other)
            {
                if (isLocked)
                {
                    ARK_ASSERT(lockPtr != nullptr);
                    lockPtr->unlockShared();
                }

                lockPtr = other.lockPtr;
                isLocked = other.isLocked;
                other.lockPtr = nullptr;
                other.isLocked = false;
            }
            return *this;
        }

        ~SharedLock()
        {
            if (isLocked)
            {
                ARK_ASSERT(lockPtr != nullptr);
                lockPtr->unlockShared();
            }
        }

        void swap(SharedLock& other)
        {
            Ark::swap(lockPtr, other.lockPtr);
            Ark::swap(isLocked, other.isLocked);
        }

        void lock()
        {
            ARK_ASSERT(lockPtr != nullptr);
            ARK_ASSERT(!isLocked);
            lockPtr->lockShared();
            isLocked = true;
        }

        bool tryLock()
        {
            ARK_ASSERT(lockPtr != nullptr);
            ARK_ASSERT(!isLocked);
            isLocked = lockPtr->tryLockShared();
            return isLocked;
        }

        void unlock()
        {
            ARK_ASSERT(lockPtr != nullptr);
            ARK_ASSERT(isLocked);
            lockPtr->unlockShared();
            isLocked = false;
        }

        SharedLockType* release()
        {
            SharedLockType* released = lockPtr;
            lockPtr = nullptr;
            isLocked = false;
            return released;
        }

        bool ownsLock() const
        {
            return isLocked;
        }

        SharedLockType* mutex() const
        {
            return lockPtr;
        }

        explicit operator bool() const
        {
            return ownsLock();
        }
    };

    /// Locks multiple lockable objects in deterministic address order.
    /// The caller must pass distinct lock objects.
    inline void lock()
    {
    }

    template <typename... LockTypes>
    void lock(LockTypes&... locks)
    {
        Ark::Tuple<LockTypes&...> lockTuple(locks...);
        auto entries = Internal::buildLockEntries(lockTuple);
        Internal::sortAndValidateLockEntries(entries);
        Internal::lockLockEntries(entries);
    }

    /// Attempts to lock multiple lockable objects without blocking.
    /// Returns `-1` on success, otherwise the original index of the first lock acquisition that failed.
    inline isize tryLock()
    {
        return -1;
    }

    template <typename... LockTypes>
    isize tryLock(LockTypes&... locks)
    {
        Ark::Tuple<LockTypes&...> lockTuple(locks...);
        auto entries = Internal::buildLockEntries(lockTuple);
        Internal::sortAndValidateLockEntries(entries);

        usize lockedCount = 0;
        for (; lockedCount < entries.getCount(); ++lockedCount)
        {
            Internal::LockEntry const& entry = entries[lockedCount];
            if (!entry.tryLockFunction(entry.address))
            {
                for (usize unlockIndex = lockedCount; unlockIndex > 0; --unlockIndex)
                {
                    Internal::LockEntry const& lockedEntry = entries[unlockIndex - 1];
                    lockedEntry.unlockFunction(lockedEntry.address);
                }

                return static_cast<isize>(entry.originalIndex);
            }
        }

        return -1;
    }

    /// Executes a callback while holding the specified lock.
    template <typename LockType, typename Callback>
    decltype(auto) withLock(LockType& lockValue, Callback&& callback)
    {
        LockGuard<LockType> guard(lockValue);

        if constexpr (Traits::isVoid<decltype(Ark::forward<Callback>(callback)())>)
        {
            Ark::forward<Callback>(callback)();
            return;
        }
        else
        {
            return Ark::forward<Callback>(callback)();
        }
    }

    /// Executes a callback while holding the specified shared lock.
    template <typename SharedLockType, typename Callback>
    decltype(auto) withSharedLock(SharedLockType& lockValue, Callback&& callback)
    {
        SharedLock<SharedLockType> guard(lockValue);

        if constexpr (Traits::isVoid<decltype(Ark::forward<Callback>(callback)())>)
        {
            Ark::forward<Callback>(callback)();
            return;
        }
        else
        {
            return Ark::forward<Callback>(callback)();
        }
    }
}
