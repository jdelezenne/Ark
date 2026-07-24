#include "Ark/Memory/Memory.hpp"
#include "Ark/Memory/StandardAllocator.hpp"

namespace Ark::Memory
{
    // Function-local static initialization is thread-safe in C++11+
    static Allocator*& globalAllocatorRef()
    {
        static Allocator* instance = nullptr;
        return instance;
    }

    Allocator& getDefaultAllocator()
    {
        Allocator*& instance = globalAllocatorRef();
        if (instance == nullptr)
        {
            static StandardAllocator fallback;
            instance = &fallback;
        }
        return *instance;
    }

    void setDefaultAllocator(Allocator& allocator)
    {
        globalAllocatorRef() = &allocator;
    }
}
