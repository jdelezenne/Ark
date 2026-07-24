#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"

namespace Ark::Memory
{
    /// Allocator optimized for small allocations using slab-based size classes.
    /// Allocations above maxSmallSize are forwarded to the parent allocator.
    class SmallObjectAllocator final : public Allocator
    {
    private:
        struct FreeNode
        {
            FreeNode* next;
        };
        struct Slab
        {
            void* base;
            Slab* next;
        };
        struct SizeClass
        {
            usize blockSize;
            FreeNode* freeList;
            Slab* slabs;
        };

        Allocator& parent;
        usize slabSize;
        usize maxSmallSize;
        static constexpr usize kMaxClasses = 8; // 8,16,32,64,128,256,512,1024 (capped by maxSmallSize)
        SizeClass classes[kMaxClasses]{};
        usize classCount{0};

    public:
        /// Creates a small-object allocator.
        /// @param parent Parent allocator used to acquire slabs and large allocations.
        /// @param maxSmallSize Maximum size handled by size classes.
        /// @param slabSize Size of each slab per class.
        SmallObjectAllocator(Allocator& parent, usize maxSmallSize = 256, usize slabSize = static_cast<usize>(16) * static_cast<usize>(1024));
        ~SmallObjectAllocator() override;

        void* allocate(uint size) override;
        void deallocate(void* ptr) override;
        void* allocateAligned(uint size, uint alignment) override;
        void deallocateAligned(void* ptr) override;
        void reset() override;

        /// Checks whether a pointer belongs to this allocator's slabs.
        /// @param ptr Pointer to test.
        /// @return True if the pointer is owned by this allocator.
        bool owns(void* ptr) const;

    private:
        SizeClass* pickClass(usize size);
        void refill(SizeClass& sc, uint alignment);
        bool ptrInClassSlabs(SizeClass const& sc, void* ptr) const;
    };
}
