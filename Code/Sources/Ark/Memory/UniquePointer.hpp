#pragma once

#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"

namespace Ark
{
#pragma region Deleters

    /// Default deleter for single objects allocated with new.
    template <typename T>
    struct DefaultDeleter final
    {
        constexpr DefaultDeleter() = default;

        template <typename U>
        constexpr DefaultDeleter(const DefaultDeleter<U>&)
        requires Traits::isConvertible<U*, T*>
        {
        }

        constexpr void operator()(T* pointer) const
        {
            static_assert(0 < sizeof(T), "Cannot delete an incomplete type");
            delete pointer;
        }
    };

    /// Default deleter specialization for dynamic arrays allocated with new[].
    template <typename T>
    struct DefaultDeleter<T[]>
    {
        constexpr DefaultDeleter() = default;

        template <typename U>
        constexpr DefaultDeleter(const DefaultDeleter<U[]>&)
        requires Traits::isConvertible<U (*)[], T (*)[]>
        {
        }

        template <typename U>
        constexpr void operator()(U* pointer) const
        requires Traits::isConvertible<U (*)[], T (*)[]>
        {
            static_assert(0 < sizeof(U), "Cannot delete an incomplete type");
            delete[] pointer;
        }
    };

    template <typename T, typename Deleter = DefaultDeleter<T>>
    struct UniquePointer;

    template <typename T, typename Deleter>
    struct UniquePointer<T[], Deleter>;

    namespace Internal
    {
        template <typename T>
        struct HasPointerType
        {
        private:
            template <typename U>
            static auto test(int) -> decltype(typename U::pointer{}, Traits::TrueType{});

            template <typename>
            static Traits::FalseType test(...);

        public:
            static constexpr bool value = decltype(test<T>(0))::value;
        };

        template <typename Deleter, typename T>
        struct PointerTypeHelper
        {
            using Type = T*;
        };

        template <typename Deleter, typename T>
        requires HasPointerType<Deleter>::value
        struct PointerTypeHelper<Deleter, T>
        {
            using Type = typename Deleter::pointer;
        };

        template <typename Deleter, typename T>
        using PointerType = typename PointerTypeHelper<Traits::RemoveReferenceType<Deleter>, T>::Type;

        template <typename T1, typename T2, bool = Traits::isEmpty<T1> && !Traits::isFinal<T1>>
        struct DeleterStorage : private T1
        {
            T2 pointer{};

            DeleterStorage() = default;

            template <typename U1, typename U2>
            DeleterStorage(U1&& d, U2&& p)
                : T1(forward<U1>(d))
                , pointer(forward<U2>(p))
            {
            }

            T1& getDeleter()
            {
                return static_cast<T1&>(*this);
            }

            T1 const& getDeleter() const
            {
                return static_cast<T1 const&>(*this);
            }

            T2& getPointer()
            {
                return pointer;
            }

            T2 const& getPointer() const
            {
                return pointer;
            }
        };

        template <typename T1, typename T2>
        struct DeleterStorage<T1, T2, false>
        {
            T1 deleter;
            T2 pointer{};

            DeleterStorage() = default;

            template <typename U1, typename U2>
            DeleterStorage(U1&& d, U2&& p)
                : deleter(forward<U1>(d))
                , pointer(forward<U2>(p))
            {
            }

            T1& getDeleter()
            {
                return deleter;
            }

            T1 const& getDeleter() const
            {
                return deleter;
            }

            T2& getPointer()
            {
                return pointer;
            }

            T2 const& getPointer() const
            {
                return pointer;
            }
        };
    }

#pragma endregion

#pragma region UniquePointer

    /// Exclusive-ownership smart pointer.
    /// Owns a single object and destroys it with the configured deleter.
    template <typename T, typename Deleter>
    struct UniquePointer final
    {
    public:
        using ElementType = T;
        using DeleterType = Deleter;
        using Pointer = Internal::PointerType<Deleter, T>;

    private:
        Internal::DeleterStorage<DeleterType, Pointer> storage;

    public:
        constexpr UniquePointer() = default;

        constexpr UniquePointer(NullType)
        {
        }

        explicit UniquePointer(Pointer p)
            : storage(DeleterType{}, p)
        {
        }

        UniquePointer(Pointer p, Traits::ConditionalType<Traits::isReference<Deleter>, Deleter, Deleter const&> d)
            : storage(d, p)
        {
        }

        UniquePointer(Pointer p, Traits::RemoveReferenceType<Deleter>&& d)
        requires(!Traits::isReference<Deleter>)
            : storage(move(d), p)
        {
        }

        UniquePointer(UniquePointer&& other)
            : storage(forward<DeleterType>(other.storage.getDeleter()), other.release())
        {
        }

        template <typename U, typename E>
        UniquePointer(UniquePointer<U, E>&& other)
        requires(Traits::isConvertible<typename UniquePointer<U, E>::Pointer, Pointer> &&
                 (!Traits::isArray<U>) &&
                 Traits::isConvertible<E, DeleterType> &&
                 !Traits::isSame<UniquePointer<U, E>, UniquePointer> &&
                 (!Traits::isReference<DeleterType> || Traits::isSame<E, DeleterType>))
            : storage(forward<E>(other.getDeleter()), other.release())
        {
        }

        ~UniquePointer()
        {
            if (storage.getPointer() != nullptr)
            {
                storage.getDeleter()(storage.getPointer());
            }
        }

        UniquePointer& operator=(UniquePointer&& other)
        {
            if (this != &other)
            {
                reset(other.release());
                storage.getDeleter() = forward<DeleterType>(other.storage.getDeleter());
            }
            return *this;
        }

        template <typename U, typename E>
        UniquePointer& operator=(UniquePointer<U, E>&& other)
        requires(Traits::isConvertible<typename UniquePointer<U, E>::Pointer, Pointer> &&
                 !Traits::isArray<U> &&
                 Traits::isAssignable<DeleterType&, E &&> &&
                 !Traits::isSame<UniquePointer<U, E>, UniquePointer>)
        {
            reset(other.release());
            storage.getDeleter() = forward<E>(other.getDeleter());
            return *this;
        }

        UniquePointer& operator=(NullType)
        {
            reset();
            return *this;
        }

        UniquePointer(UniquePointer const&) = delete;
        UniquePointer& operator=(UniquePointer const&) = delete;

        T& operator*() const
        requires(!Traits::isVoid<T>)
        {
            return *storage.getPointer();
        }

        Pointer operator->() const
        {
            return storage.getPointer();
        }

        Pointer get() const
        {
            return storage.getPointer();
        }

        DeleterType& getDeleter()
        {
            return storage.getDeleter();
        }

        DeleterType const& getDeleter() const
        {
            return storage.getDeleter();
        }

        explicit operator bool() const
        {
            return storage.getPointer() != nullptr;
        }

        Pointer release()
        {
            Pointer result = storage.getPointer();
            storage.getPointer() = nullptr;
            return result;
        }

        void reset(Pointer value = nullptr)
        {
            Pointer old = storage.getPointer();
            storage.getPointer() = value;
            if (old != nullptr)
            {
                storage.getDeleter()(old);
            }
        }

        void swap(UniquePointer& other)
        {
            Ark::swap(storage.getPointer(), other.storage.getPointer());
            Ark::swap(storage.getDeleter(), other.storage.getDeleter());
        }
    };

#pragma endregion

#pragma region UniquePointer for Arrays

    /// Exclusive-ownership smart pointer specialization for arrays.
    template <typename T, typename Deleter>
    struct UniquePointer<T[], Deleter> final
    {
    public:
        using ElementType = T;
        using DeleterType = Deleter;
        using Pointer = Internal::PointerType<Deleter, T>;

    private:
        Internal::DeleterStorage<DeleterType, Pointer> storage;

    public:
        constexpr UniquePointer() = default;

        constexpr UniquePointer(NullType)
        {
        }

        template <typename U>
        explicit UniquePointer(U p)
        requires Traits::isSame<U, Pointer> ||
                 (Traits::isSame<Pointer, ElementType*> &&
                  Traits::isPointer<U> &&
                  Traits::isConvertible<Traits::RemovePointerType<U> (*)[], ElementType (*)[]>) ||
                 Traits::isNullPointer<U>
            : storage(DeleterType{}, p)
        {
        }

        template <typename U>
        UniquePointer(U p, Traits::ConditionalType<Traits::isReference<Deleter>, Deleter, Deleter const&> d)
        requires Traits::isSame<U, Pointer> ||
                 (Traits::isSame<Pointer, ElementType*> &&
                  Traits::isPointer<U> &&
                  Traits::isConvertible<Traits::RemovePointerType<U> (*)[], ElementType (*)[]>) ||
                 Traits::isNullPointer<U>
            : storage(d, p)
        {
        }

        template <typename U>
        UniquePointer(U p, Traits::RemoveReferenceType<Deleter>&& d)
        requires(Traits::isSame<U, Pointer> ||
                 (Traits::isSame<Pointer, ElementType*> &&
                  Traits::isPointer<U> &&
                  Traits::isConvertible<Traits::RemovePointerType<U> (*)[], ElementType (*)[]>) ||
                 Traits::isNullPointer<U>) &&
                (!Traits::isReference<Deleter>)
            : storage(move(d), p)
        {
        }

        UniquePointer(UniquePointer&& other)
            : storage(forward<DeleterType>(other.storage.getDeleter()), other.release())
        {
        }

        template <typename U, typename E>
        UniquePointer(UniquePointer<U, E>&& other)
        requires(Traits::isArray<U> &&
                 Traits::isSame<Pointer, ElementType*> &&
                 Traits::isSame<typename UniquePointer<U, E>::Pointer, typename UniquePointer<U, E>::ElementType*> &&
                 Traits::isConvertible<typename UniquePointer<U, E>::ElementType (*)[], ElementType (*)[]> &&
                 Traits::isConvertible<E, DeleterType> &&
                 !Traits::isSame<UniquePointer<U, E>, UniquePointer> &&
                 (!Traits::isReference<DeleterType> || Traits::isSame<E, DeleterType>))
            : storage(forward<E>(other.getDeleter()), other.release())
        {
        }

        ~UniquePointer()
        {
            if (storage.getPointer() != nullptr)
            {
                storage.getDeleter()(storage.getPointer());
            }
        }

        UniquePointer& operator=(UniquePointer&& other)
        {
            if (this != &other)
            {
                reset(other.release());
                storage.getDeleter() = forward<DeleterType>(other.storage.getDeleter());
            }
            return *this;
        }

        template <typename U, typename E>
        UniquePointer& operator=(UniquePointer<U, E>&& other)
        requires(Traits::isArray<U> &&
                 Traits::isSame<Pointer, ElementType*> &&
                 Traits::isSame<typename UniquePointer<U, E>::Pointer, typename UniquePointer<U, E>::ElementType*> &&
                 Traits::isConvertible<typename UniquePointer<U, E>::ElementType (*)[], ElementType (*)[]> &&
                 Traits::isAssignable<DeleterType&, E &&> &&
                 !Traits::isSame<UniquePointer<U, E>, UniquePointer>)
        {
            reset(other.release());
            storage.getDeleter() = forward<E>(other.getDeleter());
            return *this;
        }

        UniquePointer& operator=(NullType)
        {
            reset();
            return *this;
        }

        UniquePointer(UniquePointer const&) = delete;
        UniquePointer& operator=(UniquePointer const&) = delete;

        T& operator[](usize index) const
        {
            return storage.getPointer()[index];
        }

        Pointer get() const
        {
            return storage.getPointer();
        }

        DeleterType& getDeleter()
        {
            return storage.getDeleter();
        }

        DeleterType const& getDeleter() const
        {
            return storage.getDeleter();
        }

        explicit operator bool() const
        {
            return storage.getPointer() != nullptr;
        }

        Pointer release()
        {
            Pointer result = storage.getPointer();
            storage.getPointer() = nullptr;
            return result;
        }

        template <typename U>
        void reset(U value)
        requires Traits::isSame<U, Pointer> ||
                 (Traits::isSame<Pointer, ElementType*> &&
                  Traits::isPointer<U> &&
                  Traits::isConvertible<Traits::RemovePointerType<U> (*)[], ElementType (*)[]>) ||
                 Traits::isNullPointer<U>
        {
            Pointer old = storage.getPointer();
            storage.getPointer() = value;
            if (old != nullptr)
            {
                storage.getDeleter()(old);
            }
        }

        void reset(NullType = nullptr)
        {
            Pointer old = storage.getPointer();
            storage.getPointer() = nullptr;
            if (old != nullptr)
            {
                storage.getDeleter()(old);
            }
        }

        void swap(UniquePointer& other)
        {
            Ark::swap(storage.getPointer(), other.storage.getPointer());
            Ark::swap(storage.getDeleter(), other.storage.getDeleter());
        }
    };

#pragma endregion

#pragma region Operators

    template <typename T1, typename D1, typename U, typename D2>
    bool operator==(UniquePointer<T1, D1> const& lhs, UniquePointer<U, D2> const& rhs)
    {
        return lhs.get() == rhs.get();
    }

    template <typename T1, typename D1, typename U, typename D2>
    bool operator!=(UniquePointer<T1, D1> const& lhs, UniquePointer<U, D2> const& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T1, typename D1, typename U, typename D2>
    bool operator<(UniquePointer<T1, D1> const& lhs, UniquePointer<U, D2> const& rhs)
    {
        return lhs.get() < rhs.get();
    }

    template <typename T1, typename D1, typename U, typename D2>
    bool operator<=(UniquePointer<T1, D1> const& lhs, UniquePointer<U, D2> const& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename T1, typename D1, typename U, typename D2>
    bool operator>(UniquePointer<T1, D1> const& lhs, UniquePointer<U, D2> const& rhs)
    {
        return rhs < lhs;
    }

    template <typename T1, typename D1, typename U, typename D2>
    bool operator>=(UniquePointer<T1, D1> const& lhs, UniquePointer<U, D2> const& rhs)
    {
        return !(lhs < rhs);
    }

    template <typename T1, typename D1, typename U2, typename D2>
    auto operator<=>(UniquePointer<T1, D1> const& lhs, UniquePointer<U2, D2> const& rhs)
    requires requires {
        Traits::CompareThreeWay{}(lhs.get(), rhs.get());
    }
    {
        return Traits::CompareThreeWay{}(lhs.get(), rhs.get());
    }

    template <typename T, typename D>
    bool operator==(UniquePointer<T, D> const& ptr, NullType)
    {
        return !ptr;
    }

    template <typename T, typename D>
    bool operator==(NullType, UniquePointer<T, D> const& ptr)
    {
        return !ptr;
    }

    template <typename T, typename D>
    bool operator!=(UniquePointer<T, D> const& ptr, NullType)
    {
        return static_cast<bool>(ptr);
    }

    template <typename T, typename D>
    bool operator!=(NullType, UniquePointer<T, D> const& ptr)
    {
        return static_cast<bool>(ptr);
    }

    template <typename T, typename D>
    bool operator<(UniquePointer<T, D> const& ptr, NullType)
    {
        return Traits::Less<typename UniquePointer<T, D>::Pointer>{}(ptr.get(), nullptr);
    }

    template <typename T, typename D>
    bool operator<(NullType, UniquePointer<T, D> const& ptr)
    {
        return Traits::Less<typename UniquePointer<T, D>::Pointer>{}(nullptr, ptr.get());
    }

    template <typename T, typename D>
    bool operator<=(UniquePointer<T, D> const& ptr, NullType)
    {
        return !(nullptr < ptr);
    }

    template <typename T, typename D>
    bool operator<=(NullType, UniquePointer<T, D> const& ptr)
    {
        return !(ptr < nullptr);
    }

    template <typename T, typename D>
    bool operator>(UniquePointer<T, D> const& ptr, NullType)
    {
        return nullptr < ptr;
    }

    template <typename T, typename D>
    bool operator>(NullType, UniquePointer<T, D> const& ptr)
    {
        return ptr < nullptr;
    }

    template <typename T, typename D>
    bool operator>=(UniquePointer<T, D> const& ptr, NullType)
    {
        return !(ptr < nullptr);
    }

    template <typename T, typename D>
    bool operator>=(NullType, UniquePointer<T, D> const& ptr)
    {
        return !(nullptr < ptr);
    }

#pragma endregion

#pragma region Functions

    template <typename T, typename D>
    void swap(UniquePointer<T, D>& lhs, UniquePointer<T, D>& rhs)
    {
        lhs.swap(rhs);
    }

    template <typename T, typename... Args>
    UniquePointer<T> makeUnique(Args&&... args)
    requires(!Traits::isArray<T>)
    {
        return UniquePointer<T>(new T(forward<Args>(args)...));
    }

    template <typename T>
    UniquePointer<T> makeUnique(usize size)
    requires Traits::isUnboundedArray<T>
    {
        return UniquePointer<T>(new Traits::RemoveExtentType<T>[size]());
    }

    template <typename T, typename... Args>
    void makeUnique(Args&&...) = delete;

    template <typename T>
    UniquePointer<T> makeUniqueForOverwrite()
    requires(!Traits::isArray<T>)
    {
        return UniquePointer<T>(new T);
    }

    template <typename T>
    UniquePointer<T> makeUniqueForOverwrite(usize size)
    requires Traits::isUnboundedArray<T>
    {
        return UniquePointer<T>(new Traits::RemoveExtentType<T>[size]);
    }

    template <typename T, typename... Args>
    void makeUniqueForOverwrite(Args&&...) = delete;

#pragma endregion
}

// hash specialization removed to avoid ambiguity on libc++
