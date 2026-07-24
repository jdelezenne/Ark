#pragma once

#include "Ark/Core/Function.hpp"
#include "Ark/Core/Macros.hpp"

namespace Ark
{
    /// Executes a callback when leaving the current scope.
    struct Defer final
    {
        ARK_NOT_COPYABLE(Defer)

        /// Callback invoked by the destructor.
        Function<void()> func;

        /// Constructs a defer guard from a callback.
        /// @param func Callback to execute on scope exit.
        Defer(Function<void()> func)
            : func(Ark::move(func))
        {
        }

        Defer(Defer&& other)
            : func(Ark::move(other.func))
        {
            other.func.clear();
        }

        Defer& operator=(Defer&& other)
        {
            if (this != &other)
            {
                if (func)
                {
                    func();
                }
                func = Ark::move(other.func);
                other.func.clear();
            }
            return *this;
        }

        /// Executes the stored callback if one is set.
        ~Defer()
        {
            if (func)
            {
                func();
            }
        }
    };

#define ARK_DEFER(func) \
    ::Ark::Defer ARK_ANONYMOUS_VARIABLE(deferFunc)((func))
}
