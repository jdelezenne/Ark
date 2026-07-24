#include "Ark/Memory/SmallObjectAllocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    static inline usize nextClassSize(usize size)
    {
        constexpr usize minClassSize = 8;
        constexpr usize maxClassSize = 1024;

        if (size <= minClassSize)
        {
            return minClassSize;
        }

        usize p2 = static_cast<usize>(nextPowerOfTwo(static_cast<uint64>(size)));
        return p2 <= maxClassSize ? p2 : maxClassSize;
    }

    SmallObjectAllocator::SmallObjectAllocator(Allocator& p, usize maxSmall, usize slab)
        : parent{p}
        , slabSize{slab}
        , maxSmallSize{maxSmall}
    {
        for (usize s = nextClassSize(1); s <= maxSmallSize && classCount < kMaxClasses; s <<= 1)
        {
            classes[classCount++] = SizeClass{s, nullptr, nullptr};
        }
    }

    SmallObjectAllocator::~SmallObjectAllocator()
    {
        reset();
    }

    SmallObjectAllocator::SizeClass* SmallObjectAllocator::pickClass(usize size)
    {
        usize b = nextClassSize(size);
        for (usize i = 0; i < classCount; ++i)
        {
            if (classes[i].blockSize == b)
            {
                return &classes[i];
            }
        }
        return nullptr;
    }

    void SmallObjectAllocator::refill(SizeClass& sc, uint alignment)
    {
        usize usable = slabSize;
        void* base = parent.allocateAligned(static_cast<uint>(usable), alignment);
        if (base == nullptr)
        {
            return;
        }
        auto* slab = static_cast<Slab*>(parent.allocate(sizeof(Slab)));
        if (slab == nullptr)
        {
            parent.deallocateAligned(base);
            return;
        }
        slab->base = base;
        slab->next = sc.slabs;
        sc.slabs = slab;

        byte* p = static_cast<byte*>(base);
        usize blocks = usable / sc.blockSize;
        for (usize i = 0; i < blocks; ++i)
        {
            auto* node = reinterpret_cast<FreeNode*>(p);
            node->next = sc.freeList;
            sc.freeList = node;
            p += sc.blockSize;
        }
    }

    bool SmallObjectAllocator::ptrInClassSlabs(SizeClass const& sc, void* ptr) const
    {
        for (Slab* s = sc.slabs; s != nullptr; s = s->next)
        {
            if (ptr >= s->base && ptr < static_cast<byte*>(s->base) + slabSize)
            {
                return true;
            }
        }
        return false;
    }

    bool SmallObjectAllocator::owns(void* ptr) const
    {
        if (ptr == nullptr)
        {
            return false;
        }

        for (usize i = 0; i < classCount; ++i)
        {
            if (ptrInClassSlabs(classes[i], ptr))
            {
                return true;
            }
        }
        return false;
    }

    void* SmallObjectAllocator::allocate(uint size)
    {
        if (size > maxSmallSize)
        {
            return parent.allocate(size);
        }
        SizeClass* sc = pickClass(size);
        if (sc == nullptr)
        {
            return parent.allocate(size);
        }
        if (sc->freeList == nullptr)
        {
            refill(*sc, static_cast<uint>(DefaultAlignment));
        }
        if (sc->freeList == nullptr)
        {
            return nullptr;
        }
        FreeNode* node = sc->freeList;
        sc->freeList = node->next;
        return node;
    }

    void SmallObjectAllocator::deallocate(void* ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }
        for (usize i = 0; i < classCount; ++i)
        {
            SizeClass& sc = classes[i];
            if (ptrInClassSlabs(sc, ptr))
            {
                auto* node = reinterpret_cast<FreeNode*>(ptr);
                node->next = sc.freeList;
                sc.freeList = node;
                return;
            }
        }
        parent.deallocate(ptr);
    }

    void* SmallObjectAllocator::allocateAligned(uint size, uint alignment)
    {
        if (size > maxSmallSize)
        {
            return parent.allocateAligned(size, alignment);
        }
        if (!isPowerOfTwo(alignment) || alignment > DefaultAlignment)
        {
            return parent.allocateAligned(size, alignment);
        }
        return allocate(size);
    }

    void SmallObjectAllocator::deallocateAligned(void* ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }

        for (usize i = 0; i < classCount; ++i)
        {
            SizeClass& sc = classes[i];
            if (ptrInClassSlabs(sc, ptr))
            {
                auto* node = reinterpret_cast<FreeNode*>(ptr);
                node->next = sc.freeList;
                sc.freeList = node;
                return;
            }
        }

        // Parent-aligned fallbacks must be released through the aligned free path.
        parent.deallocateAligned(ptr);
    }

    void SmallObjectAllocator::reset()
    {
        for (usize i = 0; i < classCount; ++i)
        {
            SizeClass& sc = classes[i];
            sc.freeList = nullptr;
            Slab* s = sc.slabs;
            while (s != nullptr)
            {
                Slab* next = s->next;
                parent.deallocateAligned(s->base);
                parent.deallocate(s);
                s = next;
            }
            sc.slabs = nullptr;
        }
    }
}
