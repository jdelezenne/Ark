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
        /// Control block used by owned/weak reference pairs (non-atomic variant).
        struct OwnedControlBlock
        {
            uint32 weakCount{1};
            bool isValid{true};

            OwnedControlBlock() = default;
            virtual ~OwnedControlBlock() = default;

            OwnedControlBlock(OwnedControlBlock const&) = delete;
            OwnedControlBlock& operator=(OwnedControlBlock const&) = delete;
            OwnedControlBlock(OwnedControlBlock&&) = delete;
            OwnedControlBlock& operator=(OwnedControlBlock&&) = delete;

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

            bool valid() const
            {
                return isValid;
            }

            void invalidate()
            {
                isValid = false;
            }

            virtual void destroy() = 0;
            virtual void deallocate() = 0;
        };

        /// Control block used by owned/weak reference pairs (atomic variant).
        struct AtomicOwnedControlBlock
        {
            Concurrency::Atomic<uint32> weakCount{1};
            Concurrency::Atomic<bool> isValid{true};

            AtomicOwnedControlBlock() = default;
            virtual ~AtomicOwnedControlBlock() = default;

            AtomicOwnedControlBlock(AtomicOwnedControlBlock const&) = delete;
            AtomicOwnedControlBlock& operator=(AtomicOwnedControlBlock const&) = delete;
            AtomicOwnedControlBlock(AtomicOwnedControlBlock&&) = delete;
            AtomicOwnedControlBlock& operator=(AtomicOwnedControlBlock&&) = delete;

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

            bool valid() const
            {
                return isValid.load(Concurrency::MemoryOrder::Acquire);
            }

            void invalidate()
            {
                isValid.store(false, Concurrency::MemoryOrder::Release);
            }

            virtual void destroy() = 0;
            virtual void deallocate() = 0;
        };

        template <bool IsAtomic>
        using OwnedControlBlockBase = Traits::ConditionalType<IsAtomic, AtomicOwnedControlBlock, OwnedControlBlock>;

        template <typename T, typename Deleter, bool IsAtomic>
        struct OwnedControlBlockPtr final : OwnedControlBlockBase<IsAtomic>
        {
            T* ptr;
            Deleter deleter;

            OwnedControlBlockPtr(T* ptr, Deleter deleter)
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

        template <typename T, bool IsAtomic>
        struct OwnedControlBlockObj final : OwnedControlBlockBase<IsAtomic>
        {
            alignas(T) uint8 storage[sizeof(T)];
            bool constructed{false};

            template <typename... Args>
            OwnedControlBlockObj(Args&&... args)
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

    template <typename T, bool IsAtomic = false>
    struct OwnedReferenceBase;

    template <typename T, bool IsAtomic = false>
    struct WeakReferenceBase;

    /// Move-only owning smart pointer with weak observer support.
    /// @tparam T Managed element type.
    /// @tparam IsAtomic Whether control block reference tracking is atomic.
    template <typename T, bool IsAtomic>
    struct OwnedReferenceBase final
    {
        template <typename U, bool IsAtomicU>
        friend struct OwnedReferenceBase;

        template <typename U, bool IsAtomicU>
        friend struct WeakReferenceBase;

    public:
        using ElementType = Traits::RemoveExtentType<T>;
        using WeakType = WeakReferenceBase<T, IsAtomic>;

    private:
        ElementType* ptr{nullptr};
        Internal::OwnedControlBlockBase<IsAtomic>* controlBlock{nullptr};

        OwnedReferenceBase(ElementType* ptr, Internal::OwnedControlBlockBase<IsAtomic>* controlBlock)
            : ptr{ptr}
            , controlBlock{controlBlock}
        {
        }

    public:
        static OwnedReferenceBase createFromControlBlock(ElementType* ptr, Internal::OwnedControlBlockBase<IsAtomic>* controlBlock)
        {
            return OwnedReferenceBase(ptr, controlBlock);
        }

    public:
        constexpr OwnedReferenceBase() = default;

        constexpr OwnedReferenceBase(NullType)
        {
        }

        template <typename U>
        explicit OwnedReferenceBase(U* ptr)
        requires Traits::isConvertible<U*, ElementType*>
        {
            if (ptr != nullptr)
            {
                Internal::OwnedControlBlockBase<IsAtomic>* cb = nullptr;
                try
                {
                    cb = new Internal::OwnedControlBlockPtr<U, DefaultDeleter<U>, IsAtomic>(ptr, DefaultDeleter<U>{});
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

        template <typename U, typename Deleter>
        OwnedReferenceBase(U* ptr, Deleter deleter)
        requires Traits::isConvertible<U*, ElementType*>
        {
            if (ptr != nullptr)
            {
                Internal::OwnedControlBlockBase<IsAtomic>* cb = nullptr;
                try
                {
                    // Copy into the control block so `deleter` remains valid if construction throws.
                    cb = new Internal::OwnedControlBlockPtr<U, Deleter, IsAtomic>(ptr, deleter);
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

        OwnedReferenceBase(OwnedReferenceBase&& other)
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            other.ptr = nullptr;
            other.controlBlock = nullptr;
        }

        template <typename U>
        OwnedReferenceBase(OwnedReferenceBase<U, IsAtomic>&& other)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            other.ptr = nullptr;
            other.controlBlock = nullptr;
        }

        ~OwnedReferenceBase()
        {
            if (controlBlock != nullptr)
            {
                controlBlock->invalidate();
                controlBlock->destroy();
                controlBlock->releaseWeakRef();
            }
        }

        OwnedReferenceBase(OwnedReferenceBase const&) = delete;
        OwnedReferenceBase& operator=(OwnedReferenceBase const&) = delete;

        OwnedReferenceBase& operator=(OwnedReferenceBase&& other)
        {
            if (this != &other)
            {
                OwnedReferenceBase temp{Ark::move(other)};
                swap(temp);
            }
            return *this;
        }

        template <typename U>
        OwnedReferenceBase& operator=(OwnedReferenceBase<U, IsAtomic>&& other)
        requires Traits::isConvertible<U*, ElementType*>
        {
            OwnedReferenceBase temp{Ark::move(other)};
            swap(temp);
            return *this;
        }

        OwnedReferenceBase& operator=(NullType)
        {
            reset();
            return *this;
        }

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

        ElementType& operator[](usize index) const
        requires Traits::isArray<T>
        {
            ARK_ASSERT(ptr != nullptr);
            return ptr[index];
        }

        ElementType* get() const
        {
            return ptr;
        }

        explicit operator bool() const
        {
            return ptr != nullptr;
        }

        void reset()
        {
            OwnedReferenceBase{}.swap(*this);
        }

        template <typename U>
        void reset(U* p)
        requires Traits::isConvertible<U*, ElementType*>
        {
            OwnedReferenceBase{p}.swap(*this);
        }

        template <typename U, typename Deleter>
        void reset(U* p, Deleter d)
        requires Traits::isConvertible<U*, ElementType*>
        {
            OwnedReferenceBase{p, Ark::move(d)}.swap(*this);
        }

        void swap(OwnedReferenceBase& other)
        {
            Ark::swap(ptr, other.ptr);
            Ark::swap(controlBlock, other.controlBlock);
        }

        ElementType* release()
        {
            if (controlBlock != nullptr)
            {
                controlBlock->invalidate();
                controlBlock->releaseWeakRef();
                controlBlock = nullptr;
            }
            ElementType* result = ptr;
            ptr = nullptr;
            return result;
        }
    };

    /// Non-owning weak observer for OwnedReferenceBase.
    /// @tparam T Managed element type.
    /// @tparam IsAtomic Whether control block reference tracking is atomic.
    template <typename T, bool IsAtomic>
    struct WeakReferenceBase final
    {
        template <typename U, bool IsAtomicU>
        friend struct WeakReferenceBase;

        template <typename U, bool IsAtomicU>
        friend struct OwnedReferenceBase;

    public:
        using ElementType = Traits::RemoveExtentType<T>;

    private:
        ElementType* ptr{nullptr};
        Internal::OwnedControlBlockBase<IsAtomic>* controlBlock{nullptr};

    public:
        constexpr WeakReferenceBase() = default;

        WeakReferenceBase(WeakReferenceBase const& other)
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            if (controlBlock != nullptr)
            {
                controlBlock->addWeakRef();
            }
        }

        template <typename U>
        WeakReferenceBase(WeakReferenceBase<U, IsAtomic> const& other)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            if (controlBlock != nullptr)
            {
                controlBlock->addWeakRef();
            }
        }

        template <typename U>
        WeakReferenceBase(OwnedReferenceBase<U, IsAtomic> const& owned)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{owned.ptr}
            , controlBlock{owned.controlBlock}
        {
            if (controlBlock != nullptr)
            {
                controlBlock->addWeakRef();
            }
        }

        WeakReferenceBase(WeakReferenceBase&& other)
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            other.ptr = nullptr;
            other.controlBlock = nullptr;
        }

        template <typename U>
        WeakReferenceBase(WeakReferenceBase<U, IsAtomic>&& other)
        requires Traits::isConvertible<U*, ElementType*>
            : ptr{other.ptr}
            , controlBlock{other.controlBlock}
        {
            other.ptr = nullptr;
            other.controlBlock = nullptr;
        }

        ~WeakReferenceBase()
        {
            if (controlBlock != nullptr)
            {
                controlBlock->releaseWeakRef();
            }
        }

        WeakReferenceBase& operator=(WeakReferenceBase const& other)
        {
            if (this != &other)
            {
                WeakReferenceBase temp{other};
                swap(temp);
            }
            return *this;
        }

        template <typename U>
        WeakReferenceBase& operator=(WeakReferenceBase<U, IsAtomic> const& other)
        requires Traits::isConvertible<U*, ElementType*>
        {
            WeakReferenceBase temp{other};
            swap(temp);
            return *this;
        }

        template <typename U>
        WeakReferenceBase& operator=(OwnedReferenceBase<U, IsAtomic> const& owned)
        requires Traits::isConvertible<U*, ElementType*>
        {
            WeakReferenceBase temp{owned};
            swap(temp);
            return *this;
        }

        WeakReferenceBase& operator=(WeakReferenceBase&& other)
        {
            if (this != &other)
            {
                WeakReferenceBase temp{Ark::move(other)};
                swap(temp);
            }
            return *this;
        }

        template <typename U>
        WeakReferenceBase& operator=(WeakReferenceBase<U, IsAtomic>&& other)
        requires Traits::isConvertible<U*, ElementType*>
        {
            WeakReferenceBase temp{Ark::move(other)};
            swap(temp);
            return *this;
        }

        bool expired() const
        {
            return controlBlock == nullptr || !controlBlock->valid();
        }

        ElementType* get() const
        {
            if (controlBlock == nullptr)
            {
                return nullptr;
            }
            if (!controlBlock->valid())
            {
                return nullptr;
            }
            return ptr;
        }

        void reset()
        {
            WeakReferenceBase{}.swap(*this);
        }

        void swap(WeakReferenceBase& other)
        {
            Ark::swap(ptr, other.ptr);
            Ark::swap(controlBlock, other.controlBlock);
        }
    };

    template <typename T>
    using OwnedReference = OwnedReferenceBase<T, false>;

    template <typename T>
    using WeakReference = WeakReferenceBase<T, false>;

    template <typename T>
    using AtomicOwnedReference = OwnedReferenceBase<T, true>;

    template <typename T>
    using AtomicWeakReference = WeakReferenceBase<T, true>;

    template <typename T, typename U, bool IsAtomic>
    bool operator==(OwnedReferenceBase<T, IsAtomic> const& lhs, OwnedReferenceBase<U, IsAtomic> const& rhs)
    {
        return lhs.get() == rhs.get();
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator!=(OwnedReferenceBase<T, IsAtomic> const& lhs, OwnedReferenceBase<U, IsAtomic> const& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator<(OwnedReferenceBase<T, IsAtomic> const& lhs, OwnedReferenceBase<U, IsAtomic> const& rhs)
    {
        return lhs.get() < rhs.get();
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator<=(OwnedReferenceBase<T, IsAtomic> const& lhs, OwnedReferenceBase<U, IsAtomic> const& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator>(OwnedReferenceBase<T, IsAtomic> const& lhs, OwnedReferenceBase<U, IsAtomic> const& rhs)
    {
        return rhs < lhs;
    }

    template <typename T, typename U, bool IsAtomic>
    bool operator>=(OwnedReferenceBase<T, IsAtomic> const& lhs, OwnedReferenceBase<U, IsAtomic> const& rhs)
    {
        return !(lhs < rhs);
    }

    template <typename T, typename U, bool IsAtomic>
    auto operator<=>(OwnedReferenceBase<T, IsAtomic> const& lhs, OwnedReferenceBase<U, IsAtomic> const& rhs)
    requires requires {
        Traits::CompareThreeWay{}(lhs.get(), rhs.get());
    }
    {
        return Traits::CompareThreeWay{}(lhs.get(), rhs.get());
    }

    template <typename T, bool IsAtomic>
    bool operator==(OwnedReferenceBase<T, IsAtomic> const& ptr, NullType)
    {
        return !ptr;
    }

    template <typename T, bool IsAtomic>
    bool operator==(NullType, OwnedReferenceBase<T, IsAtomic> const& ptr)
    {
        return !ptr;
    }

    template <typename T, bool IsAtomic>
    bool operator!=(OwnedReferenceBase<T, IsAtomic> const& ptr, NullType)
    {
        return static_cast<bool>(ptr);
    }

    template <typename T, bool IsAtomic>
    bool operator!=(NullType, OwnedReferenceBase<T, IsAtomic> const& ptr)
    {
        return static_cast<bool>(ptr);
    }

    template <typename T, bool IsAtomic>
    bool operator<(OwnedReferenceBase<T, IsAtomic> const& ptr, NullType)
    {
        return Traits::Less<typename OwnedReferenceBase<T, IsAtomic>::ElementType*>{}(ptr.get(), nullptr);
    }

    template <typename T, bool IsAtomic>
    bool operator<(NullType, OwnedReferenceBase<T, IsAtomic> const& ptr)
    {
        return Traits::Less<typename OwnedReferenceBase<T, IsAtomic>::ElementType*>{}(nullptr, ptr.get());
    }

    template <typename T, bool IsAtomic>
    bool operator<=(OwnedReferenceBase<T, IsAtomic> const& ptr, NullType)
    {
        return !(nullptr < ptr);
    }

    template <typename T, bool IsAtomic>
    bool operator<=(NullType, OwnedReferenceBase<T, IsAtomic> const& ptr)
    {
        return !(ptr < nullptr);
    }

    template <typename T, bool IsAtomic>
    bool operator>(OwnedReferenceBase<T, IsAtomic> const& ptr, NullType)
    {
        return nullptr < ptr;
    }

    template <typename T, bool IsAtomic>
    bool operator>(NullType, OwnedReferenceBase<T, IsAtomic> const& ptr)
    {
        return ptr < nullptr;
    }

    template <typename T, bool IsAtomic>
    bool operator>=(OwnedReferenceBase<T, IsAtomic> const& ptr, NullType)
    {
        return !(ptr < nullptr);
    }

    template <typename T, bool IsAtomic>
    bool operator>=(NullType, OwnedReferenceBase<T, IsAtomic> const& ptr)
    {
        return !(nullptr < ptr);
    }

    template <typename T, bool IsAtomic>
    auto operator<=>(OwnedReferenceBase<T, IsAtomic> const& ptr, NullType)
    {
        return Traits::CompareThreeWay{}(ptr.get(), static_cast<typename OwnedReferenceBase<T, IsAtomic>::ElementType*>(nullptr));
    }

    template <typename T, bool IsAtomic>
    void swap(OwnedReferenceBase<T, IsAtomic>& lhs, OwnedReferenceBase<T, IsAtomic>& rhs)
    {
        lhs.swap(rhs);
    }

    template <typename T, bool IsAtomic>
    void swap(WeakReferenceBase<T, IsAtomic>& lhs, WeakReferenceBase<T, IsAtomic>& rhs)
    {
        lhs.swap(rhs);
    }

    template <typename T, typename... Args>
    OwnedReference<T> makeOwned(Args&&... args)
    requires(!Traits::isArray<T>)
    {
        T* object = new T(Ark::forward<Args>(args)...);
        try
        {
            auto controlBlock = new Internal::OwnedControlBlockPtr<T, DefaultDeleter<T>, false>(object, DefaultDeleter<T>{});
            return OwnedReference<T>::createFromControlBlock(object, controlBlock);
        }
        catch (...)
        {
            delete object;
            throw;
        }
    }

    template <typename T, typename... Args>
    AtomicOwnedReference<T> makeAtomicOwned(Args&&... args)
    requires(!Traits::isArray<T>)
    {
        T* object = new T(Ark::forward<Args>(args)...);
        try
        {
            auto controlBlock = new Internal::OwnedControlBlockPtr<T, DefaultDeleter<T>, true>(object, DefaultDeleter<T>{});
            return AtomicOwnedReference<T>::createFromControlBlock(object, controlBlock);
        }
        catch (...)
        {
            delete object;
            throw;
        }
    }
}
