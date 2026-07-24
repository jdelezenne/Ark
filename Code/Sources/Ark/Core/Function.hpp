#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"
#include "Ark/Memory/Functions.hpp"

#include <cstddef>

#if defined(ARK_FUNCTION_USE_STANDARD_LIBRARY)
#include <functional>
#endif

namespace Ark
{
    template <typename>
    class Function;

    template <typename>
    class FunctionRef;

#pragma region Function Class Template

    /// @class Function
    /// A type-erased callable object wrapper similar to std::function
    ///
    /// Function provides a way to store callable objects (functions, lambdas, etc.)
    /// with a specific signature and invoke them later, providing type erasure.
    /// This implementation includes small object optimization to avoid heap allocations
    /// for small callable objects.
    ///
    /// @tparam Signature The function signature, e.g., `int(float, bool)`
    template <typename Signature>
    class Function;

#if defined(ARK_FUNCTION_USE_STANDARD_LIBRARY)

    /// Specialization for function signatures using std::function
    template <typename Ret, typename... Args>
    class Function<Ret(Args...)>
    {
    public:
        using StdFunction = std::function<Ret(Args...)>;

        Function() = default;

        Function(NullType)
            : func()
        {
        }

        Function(const Function&) = default;
        Function(Function&&) = default;

        template <typename F,
                  typename = Traits::EnableIfType<!Traits::isSame<Traits::RemoveReferenceType<F>, Function>>>
        Function(F&& f)
            : func(Ark::forward<F>(f))
        {
        }

        template <typename Class>
        Function(Ret (Class::*method)(Args...), Class* instance);

        template <typename Class>
        Function(Ret (Class::*method)(Args...) const, const Class* instance);

        template <typename Class>
        Function(Ret (Class::*method)(Args...) volatile, volatile Class* instance);

        template <typename Class>
        Function(Ret (Class::*method)(Args...) const volatile, const volatile Class* instance);

        ~Function() = default;

        Function& operator=(const Function&) = default;
        Function& operator=(Function&&) = default;

        Function& operator=(NullType)
        {
            func = StdFunction();
            return *this;
        }

        template <typename F,
                  typename = Traits::EnableIfType<!Traits::isSame<Traits::RemoveReferenceType<F>, Function>>>
        Function& operator=(F&& f)
        {
            func = StdFunction(Ark::forward<F>(f));
            return *this;
        }

        Ret operator()(Args... args) const
        {
            return func(Ark::forward<Args>(args)...);
        }

        explicit operator bool() const
        {
            return static_cast<bool>(func);
        }

        void clear()
        {
            func = StdFunction();
        }

        void swap(Function& other)
        {
            func.swap(other.func);
        }

        template <typename T>
        T* getTarget()
        {
            return func.template target<T>();
        }

        template <typename T>
        const T* getTarget() const
        {
            return func.template target<T>();
        }

    private:
        StdFunction func;
    };

#else

    /// Specialization for function signatures
    template <typename Ret, typename... Args>
    class Function<Ret(Args...)>
    {
    public:
        /// Construct an empty function
        Function()
            : callable(nullptr)
            , useSmallBuffer(false)
        {
        }

        /// Construct from nullptr
        Function(NullType)
            : callable(nullptr)
            , useSmallBuffer(false)
        {
        }

        Function(const Function& other)
            : callable(nullptr)
            , useSmallBuffer(false)
        {
            if (other.callable)
            {
                if (other.useSmallBuffer)
                {
                    // Clone from other's buffer to our buffer
                    callable = other.callable->clone(buffer);
                    useSmallBuffer = true;
                }
                else
                {
                    // Clone to heap
                    callable = other.callable->clone(nullptr);
                    useSmallBuffer = false;
                }
            }
        }

        Function(Function&& other)
            : callable(nullptr)
            , useSmallBuffer(false)
        {
            if (other.callable)
            {
                if (other.useSmallBuffer)
                {
                    // Clone from other's buffer to our buffer
                    callable = other.callable->clone(buffer);
                    useSmallBuffer = true;

                    other.callable->destroyInPlace();
                    other.callable = nullptr;
                    other.useSmallBuffer = false;
                }
                else
                {
                    // Take ownership of other's heap-allocated callable
                    callable = other.callable;
                    useSmallBuffer = false;
                    other.callable = nullptr;
                }
            }
        }

        /// Construct from a callable object
        template <typename F,
                  typename = Traits::EnableIfType<!Traits::isSame<Traits::RemoveReferenceType<F>, Function>>>
        Function(F&& f)
            : callable(nullptr)
            , useSmallBuffer(false)
        {
            using CleanF = Traits::RemoveReferenceType<F>;

            // Only use small buffer for appropriate sized objects
            if (sizeof(CallableInternal<CleanF>) <= SMALL_BUFFER_SIZE)
            {
                // Use small buffer
                callable = new (buffer) CallableInternal<CleanF>(Ark::forward<F>(f));
                useSmallBuffer = true;
            }
            else
            {
                // Use heap
                callable = new CallableInternal<CleanF>(Ark::forward<F>(f));
                useSmallBuffer = false;
            }
        }

        /// Construct from a member function pointer (non-const)
        template <typename Class>
        Function(Ret (Class::*func)(Args...), Class* instance);

        /// Construct from a member function pointer (const)
        template <typename Class>
        Function(Ret (Class::*func)(Args...) const, const Class* instance);

        /// Construct from a member function pointer (volatile)
        template <typename Class>
        Function(Ret (Class::*func)(Args...) volatile, volatile Class* instance);

        /// Construct from a member function pointer (const volatile)
        template <typename Class>
        Function(Ret (Class::*func)(Args...) const volatile, const volatile Class* instance);

        ~Function()
        {
            clear();
        }

        Function& operator=(const Function& other)
        {
            if (this != &other)
            {
                clear();

                if (other.callable)
                {
                    if (other.useSmallBuffer)
                    {
                        // Clone from other's buffer to our buffer
                        callable = other.callable->clone(buffer);
                        useSmallBuffer = true;
                    }
                    else
                    {
                        // Clone to heap
                        callable = other.callable->clone(nullptr);
                        useSmallBuffer = false;
                    }
                }
            }
            return *this;
        }

        Function& operator=(Function&& other)
        {
            if (this != &other)
            {
                clear();

                if (other.callable)
                {
                    if (other.useSmallBuffer)
                    {
                        // Clone from other's buffer to our buffer
                        callable = other.callable->clone(buffer);
                        useSmallBuffer = true;

                        other.callable->destroyInPlace();
                        other.callable = nullptr;
                        other.useSmallBuffer = false;
                    }
                    else
                    {
                        // Take ownership of other's heap-allocated callable
                        callable = other.callable;
                        useSmallBuffer = false;
                        other.callable = nullptr;
                    }
                }
            }
            return *this;
        }

        /// Assign nullptr
        Function& operator=(NullType)
        {
            clear();
            return *this;
        }

        /// Assign a callable object
        template <typename F,
                  typename = Traits::EnableIfType<!Traits::isSame<Traits::RemoveReferenceType<F>, Function>>>
        Function& operator=(F&& f)
        {
            Function temp(Ark::forward<F>(f));
            swap(temp);
            return *this;
        }

        /// Invoke the stored callable
        Ret operator()(Args... args) const
        {
            ARK_ASSERT_MSG(callable != nullptr, "Attempt to invoke empty Ark::Function");

            if (!callable)
            {
                ARK_FATAL_ERROR_MSG("Attempt to invoke empty Ark::Function");
            }

            return callable->invoke(Ark::forward<Args>(args)...);
        }

        /// Check if the function contains a callable
        explicit operator bool() const
        {
            return callable != nullptr;
        }

        /// Clear the function
        void clear()
        {
            if (callable)
            {
                if (useSmallBuffer)
                {
                    callable->destroyInPlace();
                }
                else
                {
                    // Delete from heap
                    delete callable;
                }

                callable = nullptr;
                useSmallBuffer = false;
            }
        }

        /// Swap with another function
        void swap(Function& other)
        {
            if (this == &other)
            {
                return;
            }

            Function temp(Ark::move(other));
            other = Ark::move(*this);
            *this = Ark::move(temp);
        }

        /// Get a pointer to the target function object
        /// @return nullptr if target cannot be accessed or if function is empty
        template <typename T>
        T* getTarget()
        {
            if (!callable)
            {
                return nullptr;
            }

            if (callable->typeId() == getTypeId<T>())
            {
                return static_cast<T*>(callable->target());
            }

            return nullptr;
        }

        /// Get a const pointer to the target function object
        /// @return nullptr if target cannot be accessed or if function is empty
        template <typename T>
        const T* getTarget() const
        {
            if (!callable)
            {
                return nullptr;
            }

            if (callable->typeId() == getTypeId<T>())
            {
                return static_cast<T const*>(callable->target());
            }

            return nullptr;
        }

#pragma region Private Types

    private:
        template <typename T>
        static void const* getTypeId()
        {
            static int key;
            return &key;
        }

        /// Size of the small buffer
        static constexpr size_t SMALL_BUFFER_SIZE = 3 * sizeof(void*);

        /// Base class for type-erased callables
        class CallableBase
        {
        public:
            /// Virtual destructor
            virtual ~CallableBase()
            {
            }

            /// Invoke the callable
            virtual Ret invoke(Args... args) const = 0;

            /// Destroy the callable in place
            virtual void destroyInPlace() = 0;

            /// Clone the callable
            /// @param buffer Buffer to clone into (for small object optimization) or nullptr for heap
            /// @return Pointer to the cloned callable
            virtual CallableBase* clone(void* buffer) const = 0;

            /// Returns a unique id for the contained callable type.
            virtual void const* typeId() const = 0;

            /// Returns a pointer to the contained callable object.
            virtual void* target() = 0;

            /// Returns a const pointer to the contained callable object.
            virtual void const* target() const = 0;
        };

        /// Implementation for a specific callable type
        template <typename F>
        class CallableInternal : public CallableBase
        {
        public:
            explicit CallableInternal(F&& f)
                : func(Ark::forward<F>(f))
            {
            }

            explicit CallableInternal(const F& f)
                : func(f)
            {
            }

            CallableInternal(const CallableInternal& other)
                : func(other.func)
            {
            }

            /// Invoke the callable
            Ret invoke(Args... args) const override
            {
                return func(Ark::forward<Args>(args)...);
            }

            void destroyInPlace() override
            {
                this->~CallableInternal();
            }

            /// Clone the callable
            CallableBase* clone(void* buffer) const override
            {
                if (buffer)
                {
                    // Clone to the provided buffer
                    return new (buffer) CallableInternal(*this);
                }
                else
                {
                    // Clone to heap
                    return new CallableInternal(*this);
                }
            }

            void const* typeId() const override
            {
                return getTypeId<F>();
            }

            void* target() override
            {
                return &func;
            }

            void const* target() const override
            {
                return &func;
            }

            F func;
        };

#pragma endregion

#pragma region Private Members

        /// Aligned buffer for small object optimization
        alignas(max_align_t) uint8 buffer[SMALL_BUFFER_SIZE];

        /// Pointer to the callable (may point to buffer or heap)
        CallableBase* callable;

        /// Flag indicating if using small buffer
        bool useSmallBuffer;

#pragma endregion
    };

#endif

#pragma endregion

#pragma region Function Comparison

    /// Compare Function with nullptr
    template <typename Ret, typename... Args>
    bool operator==(const Function<Ret(Args...)>& f, NullType)
    {
        return !f;
    }

    /// Compare nullptr with Function
    template <typename Ret, typename... Args>
    bool operator==(NullType, const Function<Ret(Args...)>& f)
    {
        return !f;
    }

    /// Compare Function with nullptr for inequality
    template <typename Ret, typename... Args>
    bool operator!=(const Function<Ret(Args...)>& f, NullType)
    {
        return static_cast<bool>(f);
    }

    /// Compare nullptr with Function for inequality
    template <typename Ret, typename... Args>
    bool operator!=(NullType, const Function<Ret(Args...)>& f)
    {
        return static_cast<bool>(f);
    }

#pragma endregion

#pragma region Member Function Pointer Support

    /// Helper for member function pointer specialization
    template <typename Class, typename Ret, typename... Args>
    struct MemberFunctionWrapper
    {
        MemberFunctionWrapper(Ret (Class::*func)(Args...), Class* instance)
            : func(func)
            , instance(instance)
        {
        }

        Ret operator()(Args... args) const
        {
            return (instance->*func)(Ark::forward<Args>(args)...);
        }

        Ret (Class::*func)(Args...);
        Class* instance;
    };

    /// Helper for const member function pointer specialization
    template <typename Class, typename Ret, typename... Args>
    struct ConstMemberFunctionWrapper
    {
        ConstMemberFunctionWrapper(Ret (Class::*func)(Args...) const, const Class* instance)
            : func(func)
            , instance(instance)
        {
        }

        Ret operator()(Args... args) const
        {
            return (instance->*func)(Ark::forward<Args>(args)...);
        }

        Ret (Class::*func)(Args...) const;
        const Class* instance;
    };

    /// Helper for volatile member function pointer specialization
    template <typename Class, typename Ret, typename... Args>
    struct VolatileMemberFunctionWrapper
    {
        VolatileMemberFunctionWrapper(Ret (Class::*func)(Args...) volatile, volatile Class* instance)
            : func(func)
            , instance(instance)
        {
        }

        Ret operator()(Args... args) const
        {
            return (instance->*func)(Ark::forward<Args>(args)...);
        }

        Ret (Class::*func)(Args...) volatile;
        volatile Class* instance;
    };

    /// Helper for const volatile member function pointer specialization
    template <typename Class, typename Ret, typename... Args>
    struct ConstVolatileMemberFunctionWrapper
    {
        ConstVolatileMemberFunctionWrapper(Ret (Class::*func)(Args...) const volatile, const volatile Class* instance)
            : func(func)
            , instance(instance)
        {
        }

        Ret operator()(Args... args) const
        {
            return (instance->*func)(Ark::forward<Args>(args)...);
        }

        Ret (Class::*func)(Args...) const volatile;
        const volatile Class* instance;
    };

    /// Constructor from member function pointer
    template <typename Ret, typename... Args>
    template <typename Class>
    Function<Ret(Args...)>::Function(Ret (Class::*func)(Args...), Class* instance)
        : Function(MemberFunctionWrapper<Class, Ret, Args...>(func, instance))
    {
    }

    /// Constructor from const member function pointer
    template <typename Ret, typename... Args>
    template <typename Class>
    Function<Ret(Args...)>::Function(Ret (Class::*func)(Args...) const, const Class* instance)
        : Function(ConstMemberFunctionWrapper<Class, Ret, Args...>(func, instance))
    {
    }

    /// Constructor from volatile member function pointer
    template <typename Ret, typename... Args>
    template <typename Class>
    Function<Ret(Args...)>::Function(Ret (Class::*func)(Args...) volatile, volatile Class* instance)
        : Function(VolatileMemberFunctionWrapper<Class, Ret, Args...>(func, instance))
    {
    }

    /// Constructor from const volatile member function pointer
    template <typename Ret, typename... Args>
    template <typename Class>
    Function<Ret(Args...)>::Function(Ret (Class::*func)(Args...) const volatile, const volatile Class* instance)
        : Function(ConstVolatileMemberFunctionWrapper<Class, Ret, Args...>(func, instance))
    {
    }

#pragma endregion

#pragma region FunctionRef Class

    /// @class FunctionRef
    /// A non-owning reference to a callable object
    ///
    /// FunctionRef provides a lightweight, non-owning wrapper around callable objects
    /// that can be used to pass callable references without taking ownership.
    /// Unlike Function, FunctionRef does not store or manage the lifetime of the callable object.
    /// The callable must outlive the FunctionRef that references it.
    ///
    /// @tparam Signature The function signature, e.g., `int(float, bool)`
    template <typename Signature>
    class FunctionRef;

    /// Specialization for function signatures
    template <typename Ret, typename... Args>
    class FunctionRef<Ret(Args...)>
    {
    public:
        /// Deleted default constructor - must reference a callable
        FunctionRef() = delete;

        /// Deleted nullptr constructor - must reference a callable
        FunctionRef(NullType) = delete;

        FunctionRef(FunctionRef const& other)
            : ownedCallable(other.ownsCallable ? other.ownedCallable : Function<Ret(Args...)>{})
            , callable(other.ownsCallable ? nullptr : other.callable)
            , invoker(other.invoker)
            , ownsCallable(other.ownsCallable)
        {
            rebindOwnedCallable();
        }

        FunctionRef(FunctionRef&& other)
            : ownedCallable(other.ownsCallable ? Ark::move(other.ownedCallable) : Function<Ret(Args...)>{})
            , callable(other.ownsCallable ? nullptr : other.callable)
            , invoker(other.invoker)
            , ownsCallable(other.ownsCallable)
        {
            rebindOwnedCallable();
            other.callable = nullptr;
            other.invoker = nullptr;
            other.ownsCallable = false;
        }

        /// Construct from an lvalue callable. Rvalues are rejected to avoid dangling references.
        template <typename F>
        requires(!Traits::isSame<Traits::RemoveCVReferenceType<F>, FunctionRef> &&
                 !Traits::isSame<Traits::RemoveCVReferenceType<F>, Function<Ret(Args...)>>)
        FunctionRef(F& callableObject)
            : callable(Ark::addressOf(callableObject))
            , invoker([](void const* obj, Args... args) -> Ret
                      {
                          return (*static_cast<F*>(const_cast<void*>(obj)))(Ark::forward<Args>(args)...);
                      })
            , ownsCallable(false)
        {
        }

        /// Construct from member function pointer (non-const)
        template <typename Class>
        FunctionRef(Ret (Class::*func)(Args...), Class* instance)
            : ownedCallable(Function<Ret(Args...)>(func, instance))
            , callable(nullptr)
            , invoker(&invokeOwned)
            , ownsCallable(true)
        {
            rebindOwnedCallable();
        }

        /// Construct from member function pointer (const)
        template <typename Class>
        FunctionRef(Ret (Class::*func)(Args...) const, const Class* instance)
            : ownedCallable(Function<Ret(Args...)>(func, instance))
            , callable(nullptr)
            , invoker(&invokeOwned)
            , ownsCallable(true)
        {
            rebindOwnedCallable();
        }

        /// Construct from Function
        FunctionRef(Function<Ret(Args...)> const& func)
            : callable(Ark::addressOf(func))
            , invoker(&invokeOwned)
            , ownsCallable(false)
        {
        }

        /// Invoke the referenced callable
        Ret operator()(Args... args) const
        {
            ARK_ASSERT_MSG(callable != nullptr && invoker != nullptr, "Attempt to invoke empty Ark::FunctionRef");
            return invoker(callable, Ark::forward<Args>(args)...);
        }

        FunctionRef& operator=(FunctionRef const& other)
        {
            if (this == &other)
            {
                return *this;
            }

            ownedCallable = other.ownsCallable ? other.ownedCallable : Function<Ret(Args...)>{};
            callable = other.ownsCallable ? nullptr : other.callable;
            invoker = other.invoker;
            ownsCallable = other.ownsCallable;
            rebindOwnedCallable();
            return *this;
        }

        FunctionRef& operator=(FunctionRef&& other)
        {
            if (this == &other)
            {
                return *this;
            }

            ownedCallable = other.ownsCallable ? Ark::move(other.ownedCallable) : Function<Ret(Args...)>{};
            callable = other.ownsCallable ? nullptr : other.callable;
            invoker = other.invoker;
            ownsCallable = other.ownsCallable;
            rebindOwnedCallable();

            other.callable = nullptr;
            other.invoker = nullptr;
            other.ownsCallable = false;
            return *this;
        }

    private:
        static Ret invokeOwned(void const* obj, Args... args)
        {
            return (*static_cast<Function<Ret(Args...)> const*>(obj))(Ark::forward<Args>(args)...);
        }

        void rebindOwnedCallable()
        {
            if (ownsCallable)
            {
                callable = Ark::addressOf(ownedCallable);
            }
        }

        /// Optional owned callable used for member function pointer constructors.
        Function<Ret(Args...)> ownedCallable{};

        /// Pointer to the callable object (non-owning unless `ownsCallable` is true)
        void const* callable{nullptr};

        /// Function pointer to invoke the callable with the correct type
        Ret (*invoker)(void const*, Args...){nullptr};

        /// True when `callable` refers to `ownedCallable`.
        bool ownsCallable{false};
    };

#pragma endregion
}
