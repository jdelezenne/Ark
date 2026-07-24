#pragma once

#include "Ark/Concurrency/Atomic.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"

namespace Ark
{
    template <typename T>
    struct DefaultDeleter;

    namespace Internal
    {
        // Non-atomic control block
        /// Reference-count control block for non-atomic shared/weak pointers.
        struct ControlBlock
        {
            uint32 sharedCount{1};
            uint32 weakCount{1};

            ControlBlock() = default;
            virtual ~ControlBlock() = default;

            // Non-copyable, non-movable
            ControlBlock(ControlBlock const&) = delete;
            ControlBlock& operator=(ControlBlock const&) = delete;
            ControlBlock(ControlBlock&&) = delete;
            ControlBlock& operator=(ControlBlock&&) = delete;

            void addSharedPointer()
            {
                ++sharedCount;
            }

            void releaseSharedPointer()
            {
                if (--sharedCount == 0)
                {
                    destroy();
                    releaseWeakRef();
                }
            }

            bool tryLockShared()
            {
                if (sharedCount == 0)
                    return false;
                ++sharedCount;
                return true;
            }

            uint32 getSharedCount() const
            {
                return sharedCount;
            }

            void addWeakRef()
            {
                ++weakCount;
            }

            void releaseWeakRef()
            {
                if (--weakCount == 0)
                {
                    deallocate();
                }
            }

            virtual void destroy() = 0;
            virtual void deallocate() = 0;
        };

        // Atomic control block
        /// Reference-count control block for atomic shared/weak pointers.
        struct AtomicControlBlock
        {
            Concurrency::Atomic<uint32> sharedCount{1};
            Concurrency::Atomic<uint32> weakCount{1};

            AtomicControlBlock() = default;
            virtual ~AtomicControlBlock() = default;

            // Non-copyable, non-movable
            AtomicControlBlock(AtomicControlBlock const&) = delete;
            AtomicControlBlock& operator=(AtomicControlBlock const&) = delete;
            AtomicControlBlock(AtomicControlBlock&&) = delete;
            AtomicControlBlock& operator=(AtomicControlBlock&&) = delete;

            void addSharedPointer()
            {
                sharedCount.fetchAdd(1, Concurrency::MemoryOrder::Relaxed);
            }

            void releaseSharedPointer()
            {
                if (sharedCount.fetchSubtract(1, Concurrency::MemoryOrder::AcquireRelease) == 1)
                {
                    destroy();
                    releaseWeakRef();
                }
            }

            void addWeakRef()
            {
                weakCount.fetchAdd(1, Concurrency::MemoryOrder::Relaxed);
            }

            void releaseWeakRef()
            {
                if (weakCount.fetchSubtract(1, Concurrency::MemoryOrder::AcquireRelease) == 1)
                {
                    deallocate();
                }
            }

            uint32 getSharedCount() const
            {
                return sharedCount.load(Concurrency::MemoryOrder::Relaxed);
            }

            bool tryLockShared()
            {
                uint32 count = sharedCount.load(Concurrency::MemoryOrder::Relaxed);
                do
                {
                    if (count == 0)
                        return false;
                }
                while (!sharedCount.compareExchange(count, count + 1, Concurrency::MemoryOrder::AcquireRelease));
                return true;
            }

            virtual void destroy() = 0;
            virtual void deallocate() = 0;
        };

        // Choose control block type based on atomic flag
        template <bool IsAtomic>
        using ControlBlockBase = typename Traits::ConditionalType<IsAtomic, AtomicControlBlock, ControlBlock>;

        // Control block for pointer + deleter
        template <typename T, typename Deleter, bool IsAtomic>
        struct ControlBlockPtr final : ControlBlockBase<IsAtomic>
        {
            T* ptr;
            Deleter deleter;

            ControlBlockPtr(T* ptr, Deleter deleter)
                : ptr{ptr}
                , deleter{Ark::move(deleter)}
            {
            }

            void destroy() override
            {
                if (ptr != nullptr)
                {
                    Deleter d = Ark::move(deleter);
                    d(ptr);
                    ptr = nullptr;
                }
            }

            void deallocate() override
            {
                delete this;
            }
        };

        // Control block for in-place constructed objects
        template <typename T, bool IsAtomic>
        struct ControlBlockObj final : ControlBlockBase<IsAtomic>
        {
            alignas(T) uint8 storage[sizeof(T)];
            bool constructed{false};

            template <typename... Args>
            ControlBlockObj(Args&&... args)
            {
                new (storage) T(Ark::forward<Args>(args)...);
                constructed = true;
            }

            T* get()
            {
                return reinterpret_cast<T*>(storage);
            }

            void destroy() override
            {
                if (constructed)
                {
                    get()->~T();
                    constructed = false;
                }
            }

            void deallocate() override
            {
                delete this;
            }
        };
    }

    // Forward declarations
    template <typename T, bool IsAtomic = false>
    struct SharedPointerBase;

    template <typename T, bool IsAtomic = false>
    struct WeakPointerBase;

    /// Shared-ownership smart pointer base implementation.
    /// @tparam T Managed element type.
    /// @tparam IsAtomic Whether control block counters are atomic.
    template <typename T, bool IsAtomic>
    struct SharedPointerBase final
    {
        template <typename U, bool IsAtomicU>
        friend struct SharedPointerBase;

        template <typename U, bool IsAtomicU>
        friend struct WeakPointerBase;

    public:
        using ElementType = Traits::RemoveExtentType<T>;
        using WeakType = WeakPointerBase<T, IsAtomic>;

    private:
        ElementType* ptr{nullptr};
        Internal::ControlBlockBase<IsAtomic>* controlBlock{nullptr};

        // Private constructor for internal use
        SharedPointerBase(ElementType* ptr, Internal::ControlBlockBase<IsAtomic>* controlBlock)
            : ptr{ptr}
            , controlBlock{controlBlock}
        {
        }

    public:
        static SharedPointerBase createFromControlBlock(ElementType* ptr, Internal::ControlBlockBase<IsAtomic>* controlBlock)
        {
            return SharedPointerBase(ptr, controlBlock);
        }

    public:
        constexpr SharedPointerBase() = default;

        constexpr SharedPointerBase(NullType)
        {
        }

        // Constructor from raw pointer
        template <typename U>
        explicit SharedPointerBase(U* ptr)
        requires Traits::isConvertible<U*, ElementType*>
        {
            if (ptr != nullptr)
            {
                Internal::ControlBlockBase<IsAtomic>* cb = nullptr;
                try
                {
                    cb = new Internal::ControlBlockPtr<U, DefaultDeleter<U>, IsAtomic>(ptr, DefaultDeleter<U>{});
                }
                catch (...)
                {
                    DefaultDeleter<U>{}(ptr);
                    throw;
                }
                this->controlBlock = cb;
                this->ptr = ptr;
            }
        }

        // Constructor with custom deleter
        template <typename U, typename Deleter>
        SharedPointerBase(U* ptr, Deleter deleter)
        requires Traits::isConvertible<U*, ElementType*>
        {
            if (ptr != nullptr)
            {
                Internal::ControlBlockBase<IsAtomic>* cb = nullptr;
                try
                {
                    // Copy into the control block so `deleter` remains valid if construction throws.
                    cb = new Internal::ControlBlockPtr<U, Deleter, IsAtomic>(ptr, deleter);
                }
                catch (...)
                {
                    deleter(ptr);
                    throw;
                }
                this->controlBlock = cb;
                this->ptr = ptr;
            }
        }

        // Aliasing constructor
        template <typename U>
        SharedPointerBase(SharedPointerBase<U, IsAtomic> const& other, ElementType* ptr)
        {
            if (other.controlBlock != nullptr)
            {
                this->ptr = ptr;
                this->controlBlock = other.controlBlock;
                this->controlBlock->addSharedPointer();
            }
        }

        // Copy constructor
        SharedPointerBase(SharedPointerBase const& other)
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            if (controlBlock != nullptr)
            {
                controlBlock->addSharedPointer();
            }
        }

        // Converting copy constructor
        template <typename U>
        SharedPointerBase(SharedPointerBase<U, IsAtomic> const& other)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            if (controlBlock != nullptr)
            {
                controlBlock->addSharedPointer();
            }
        }

        // Move constructor
        SharedPointerBase(SharedPointerBase&& other)
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            other.ptr = nullptr;
            other.controlBlock = nullptr;
        }

        // Converting move constructor
        template <typename U>
        SharedPointerBase(SharedPointerBase<U, IsAtomic>&& other)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            other.ptr = nullptr;
            other.controlBlock = nullptr;
        }

        // Constructor from weak pointer
        explicit SharedPointerBase(WeakPointerBase<T, IsAtomic> const& weak)
        {
            if (weak.controlBlock != nullptr && weak.controlBlock->tryLockShared())
            {
                ptr = weak.ptr;
                controlBlock = weak.controlBlock;
            }
        }

        // Destructor
        ~SharedPointerBase()
        {
            if (controlBlock != nullptr)
            {
                controlBlock->releaseSharedPointer();
            }
        }

        // Copy assignment
        SharedPointerBase& operator=(SharedPointerBase const& other)
        {
            if (this != &other)
            {
                SharedPointerBase temp{other};
                swap(temp);
            }
            return *this;
        }

        // Converting copy assignment
        template <typename U>
        SharedPointerBase& operator=(SharedPointerBase<U, IsAtomic> const& other)
        requires Traits::isConvertible<U*, ElementType*>
        {
            SharedPointerBase temp{other};
            swap(temp);
            return *this;
        }

        // Move assignment
        SharedPointerBase& operator=(SharedPointerBase&& other)
        {
            if (this != &other)
            {
                SharedPointerBase temp{Ark::move(other)};
                swap(temp);
            }
            return *this;
        }

        // Converting move assignment
        template <typename U>
        SharedPointerBase& operator=(SharedPointerBase<U, IsAtomic>&& other)
        requires Traits::isConvertible<U*, ElementType*>
        {
            SharedPointerBase temp{Ark::move(other)};
            swap(temp);
            return *this;
        }

        // Null assignment
        SharedPointerBase& operator=(NullType)
        {
            reset();
            return *this;
        }

        // Dereference operators
        ElementType& operator*() const
        requires(!Traits::isVoid<ElementType>)
        {
            ARK_ASSERT(ptr != nullptr);
            return *ptr;
        }

        ElementType* operator->() const
        {
            return ptr;
        }

        // Array access operator
        ElementType& operator[](usize index) const
        requires Traits::isArray<T>
        {
            ARK_ASSERT(ptr != nullptr);
            return ptr[index];
        }

        // Observers
        ElementType* get() const
        {
            return ptr;
        }

        uint32 useCount() const
        {
            return controlBlock != nullptr ? controlBlock->getSharedCount() : 0;
        }

        bool unique() const
        {
            return useCount() == 1;
        }

        explicit operator bool() const
        {
            return ptr != nullptr;
        }

        // Modifiers
        void reset()
        {
            SharedPointerBase{}.swap(*this);
        }

        template <typename U>
        void reset(U* p)
        requires Traits::isConvertible<U*, ElementType*>
        {
            SharedPointerBase{p}.swap(*this);
        }

        template <typename U, typename Deleter>
        void reset(U* p, Deleter d)
        requires Traits::isConvertible<U*, ElementType*>
        {
            SharedPointerBase{p, Ark::move(d)}.swap(*this);
        }

        void swap(SharedPointerBase& other)
        {
            Ark::swap(ptr, other.ptr);
            Ark::swap(controlBlock, other.controlBlock);
        }

        // Owner-based ordering
        template <typename U>
        bool ownerBefore(SharedPointerBase<U, IsAtomic> const& other) const
        {
            return controlBlock < other.controlBlock;
        }

        template <typename U>
        bool ownerBefore(WeakPointerBase<U, IsAtomic> const& other) const
        {
            return controlBlock < other.controlBlock;
        }
    };

    /// Non-owning weak observer for SharedPointerBase.
    /// @tparam T Managed element type.
    /// @tparam IsAtomic Whether control block counters are atomic.
    template <typename T, bool IsAtomic>
    struct WeakPointerBase final
    {
        template <typename U, bool IsAtomicU>
        friend struct WeakPointerBase;

        template <typename U, bool IsAtomicU>
        friend struct SharedPointerBase;

    public:
        using ElementType = Traits::RemoveExtentType<T>;

    private:
        ElementType* ptr{nullptr};
        Internal::ControlBlockBase<IsAtomic>* controlBlock{nullptr};

    public:
        constexpr WeakPointerBase() = default;

        // Copy constructor
        WeakPointerBase(WeakPointerBase const& other)
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            if (controlBlock != nullptr)
            {
                controlBlock->addWeakRef();
            }
        }

        // Converting copy constructor
        template <typename U>
        WeakPointerBase(WeakPointerBase<U, IsAtomic> const& other)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            if (controlBlock != nullptr)
            {
                controlBlock->addWeakRef();
            }
        }

        // Constructor from SharedPointer
        template <typename U>
        WeakPointerBase(SharedPointerBase<U, IsAtomic> const& shared)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{shared.ptr}
            , controlBlock{shared.controlBlock}
        {
            if (controlBlock != nullptr)
            {
                controlBlock->addWeakRef();
            }
        }

        // Move constructor
        WeakPointerBase(WeakPointerBase&& other)
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            other.ptr = nullptr;
            other.controlBlock = nullptr;
        }

        // Converting move constructor
        template <typename U>
        WeakPointerBase(WeakPointerBase<U, IsAtomic>&& other)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            other.ptr = nullptr;
            other.controlBlock = nullptr;
        }

        // Destructor
        ~WeakPointerBase()
        {
            if (controlBlock != nullptr)
            {
                controlBlock->releaseWeakRef();
            }
        }

        // Copy assignment
        WeakPointerBase& operator=(WeakPointerBase const& other)
        {
            if (this != &other)
            {
                WeakPointerBase temp{other};
                swap(temp);
            }
            return *this;
        }

        // Converting copy assignment
        template <typename U>
        WeakPointerBase& operator=(WeakPointerBase<U, IsAtomic> const& other)
        requires Traits::isConvertible<U*, ElementType*>
        {
            WeakPointerBase temp{other};
            swap(temp);
            return *this;
        }

        // Assignment from SharedPointer
        template <typename U>
        WeakPointerBase& operator=(SharedPointerBase<U, IsAtomic> const& shared)
        requires Traits::isConvertible<U*, ElementType*>
        {
            WeakPointerBase temp{shared};
            swap(temp);
            return *this;
        }

        // Move assignment
        WeakPointerBase& operator=(WeakPointerBase&& other)
        {
            if (this != &other)
            {
                WeakPointerBase temp{Ark::move(other)};
                swap(temp);
            }
            return *this;
        }

        // Converting move assignment
        template <typename U>
        WeakPointerBase& operator=(WeakPointerBase<U, IsAtomic>&& other)
        requires Traits::isConvertible<U*, ElementType*>
        {
            WeakPointerBase temp{Ark::move(other)};
            swap(temp);
            return *this;
        }

        // Observers
        uint32 useCount() const
        {
            return controlBlock != nullptr ? controlBlock->getSharedCount() : 0;
        }

        bool expired() const
        {
            return useCount() == 0;
        }

        SharedPointerBase<T, IsAtomic> lock() const
        {
            if (controlBlock != nullptr && controlBlock->tryLockShared())
            {
                return SharedPointerBase<T, IsAtomic>{ptr, controlBlock};
            }
            return SharedPointerBase<T, IsAtomic>{};
        }

        // Modifiers
        void reset()
        {
            WeakPointerBase{}.swap(*this);
        }

        void swap(WeakPointerBase& other)
        {
            Ark::swap(ptr, other.ptr);
            Ark::swap(controlBlock, other.controlBlock);
        }

        // Owner-based ordering
        template <typename U>
        bool ownerBefore(WeakPointerBase<U, IsAtomic> const& other) const
        {
            return controlBlock < other.controlBlock;
        }

        template <typename U>
        bool ownerBefore(SharedPointerBase<U, IsAtomic> const& other) const
        {
            return controlBlock < other.controlBlock;
        }
    };

    // Public type aliases
    template <typename T>
    using SharedPointer = SharedPointerBase<T, false>;

    template <typename T>
    using WeakPointer = WeakPointerBase<T, false>;

    template <typename T>
    using AtomicSharedPointer = SharedPointerBase<T, true>;

    template <typename T>
    using AtomicWeakPointer = WeakPointerBase<T, true>;

    // Comparison operators for SharedPointer
    template <typename T, typename U, bool IsAtomic>
    bool operator==(SharedPointerBase<T, IsAtomic> const& lhs, SharedPointerBase<U, IsAtomic> const& rhs)
    {
        return lhs.get() == rhs.get();
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator!=(SharedPointerBase<T, IsAtomic> const& lhs, SharedPointerBase<U, IsAtomic> const& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator<(SharedPointerBase<T, IsAtomic> const& lhs, SharedPointerBase<U, IsAtomic> const& rhs)
    {
        return lhs.get() < rhs.get();
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator<=(SharedPointerBase<T, IsAtomic> const& lhs, SharedPointerBase<U, IsAtomic> const& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator>(SharedPointerBase<T, IsAtomic> const& lhs, SharedPointerBase<U, IsAtomic> const& rhs)
    {
        return rhs < lhs;
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator>=(SharedPointerBase<T, IsAtomic> const& lhs, SharedPointerBase<U, IsAtomic> const& rhs)
    {
        return !(lhs < rhs);
    }

    template <typename T, typename U, bool IsAtomic>
    auto operator<=>(SharedPointerBase<T, IsAtomic> const& lhs, SharedPointerBase<U, IsAtomic> const& rhs)
    requires requires {
        Traits::CompareThreeWay{}(lhs.get(), rhs.get());
    }
    {
        return Traits::CompareThreeWay{}(lhs.get(), rhs.get());
    }

    template <typename T, bool IsAtomic>
    bool operator==(SharedPointerBase<T, IsAtomic> const& ptr, NullType)
    {
        return !ptr;
    }

    template <typename T, bool IsAtomic>
    bool operator==(NullType, SharedPointerBase<T, IsAtomic> const& ptr)
    {
        return !ptr;
    }

    template <typename T, bool IsAtomic>
    bool operator!=(SharedPointerBase<T, IsAtomic> const& ptr, NullType)
    {
        return static_cast<bool>(ptr);
    }

    template <typename T, bool IsAtomic>
    bool operator<(SharedPointerBase<T, IsAtomic> const& ptr, NullType)
    {
        return Traits::Less<typename SharedPointerBase<T, IsAtomic>::ElementType*>{}(ptr.get(), nullptr);
    }

    template <typename T, bool IsAtomic>
    bool operator<(NullType, SharedPointerBase<T, IsAtomic> const& ptr)
    {
        return Traits::Less<typename SharedPointerBase<T, IsAtomic>::ElementType*>{}(nullptr, ptr.get());
    }

    template <typename T, bool IsAtomic>
    bool operator<=(SharedPointerBase<T, IsAtomic> const& ptr, NullType)
    {
        return !(nullptr < ptr);
    }

    template <typename T, bool IsAtomic>
    bool operator<=(NullType, SharedPointerBase<T, IsAtomic> const& ptr)
    {
        return !(ptr < nullptr);
    }

    template <typename T, bool IsAtomic>
    bool operator>(SharedPointerBase<T, IsAtomic> const& ptr, NullType)
    {
        return nullptr < ptr;
    }

    template <typename T, bool IsAtomic>
    bool operator>(NullType, SharedPointerBase<T, IsAtomic> const& ptr)
    {
        return ptr < nullptr;
    }

    template <typename T, bool IsAtomic>
    bool operator>=(SharedPointerBase<T, IsAtomic> const& ptr, NullType)
    {
        return !(ptr < nullptr);
    }

    template <typename T, bool IsAtomic>
    bool operator>=(NullType, SharedPointerBase<T, IsAtomic> const& ptr)
    {
        return !(nullptr < ptr);
    }

    template <typename T, bool IsAtomic>
    auto operator<=>(SharedPointerBase<T, IsAtomic> const& ptr, NullType)
    {
        return Traits::CompareThreeWay{}(ptr.get(), static_cast<typename SharedPointerBase<T, IsAtomic>::ElementType*>(nullptr));
    }

    // Utility functions
    template <typename T, bool IsAtomic>
    void swap(SharedPointerBase<T, IsAtomic>& lhs, SharedPointerBase<T, IsAtomic>& rhs)
    {
        lhs.swap(rhs);
    }

    template <typename T, bool IsAtomic>
    void swap(WeakPointerBase<T, IsAtomic>& lhs, WeakPointerBase<T, IsAtomic>& rhs)
    {
        lhs.swap(rhs);
    }

    // Factory functions
    template <typename T, typename... Args>
    SharedPointer<T> makeShared(Args&&... args)
    requires(!Traits::isArray<T>)
    {
        auto controlBlock = new Internal::ControlBlockObj<T, false>(Ark::forward<Args>(args)...);
        return SharedPointer<T>::createFromControlBlock(controlBlock->get(), controlBlock);
    }

    template <typename T, typename... Args>
    AtomicSharedPointer<T> makeAtomicShared(Args&&... args)
    requires(!Traits::isArray<T>)
    {
        auto controlBlock = new Internal::ControlBlockObj<T, true>(Ark::forward<Args>(args)...);
        return AtomicSharedPointer<T>::createFromControlBlock(controlBlock->get(), controlBlock);
    }

    // Cast functions
    template <typename T, typename U, bool IsAtomic>
    SharedPointerBase<T, IsAtomic> staticPointerCast(SharedPointerBase<U, IsAtomic> const& shared)
    {
        auto ptr = static_cast<typename SharedPointerBase<T, IsAtomic>::ElementType*>(shared.get());
        return SharedPointerBase<T, IsAtomic>{shared, ptr};
    }

    template <typename T, typename U, bool IsAtomic>
    SharedPointerBase<T, IsAtomic> dynamicPointerCast(SharedPointerBase<U, IsAtomic> const& shared)
    {
        auto ptr = dynamic_cast<typename SharedPointerBase<T, IsAtomic>::ElementType*>(shared.get());
        if (ptr != nullptr)
        {
            return SharedPointerBase<T, IsAtomic>{shared, ptr};
        }
        return SharedPointerBase<T, IsAtomic>{};
    }

    template <typename T, typename U, bool IsAtomic>
    SharedPointerBase<T, IsAtomic> constPointerCast(SharedPointerBase<U, IsAtomic> const& shared)
    {
        auto ptr = const_cast<typename SharedPointerBase<T, IsAtomic>::ElementType*>(shared.get());
        return SharedPointerBase<T, IsAtomic>{shared, ptr};
    }

    template <typename T, typename U, bool IsAtomic>
    SharedPointerBase<T, IsAtomic> reinterpretPointerCast(SharedPointerBase<U, IsAtomic> const& shared)
    {
        auto ptr = reinterpret_cast<typename SharedPointerBase<T, IsAtomic>::ElementType*>(shared.get());
        return SharedPointerBase<T, IsAtomic>{shared, ptr};
    }
}

// hash specializations removed for now to avoid ambiguous lookup with libc++
