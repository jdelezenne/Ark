#pragma once

#include "Ark/Concurrency/Types.hpp"

#include <atomic>

namespace Ark::Memory
{
    inline void barrier(Concurrency::MemoryOrder order)
    {
        switch (order)
        {
            case Concurrency::MemoryOrder::Relaxed:
                std::atomic_thread_fence(std::memory_order_relaxed);
                break;

            case Concurrency::MemoryOrder::Acquire:
                std::atomic_thread_fence(std::memory_order_acquire);
                break;

            case Concurrency::MemoryOrder::Release:
                std::atomic_thread_fence(std::memory_order_release);
                break;

            case Concurrency::MemoryOrder::AcquireRelease:
                std::atomic_thread_fence(std::memory_order_acq_rel);
                break;

            case Concurrency::MemoryOrder::SequentiallyConsistent:
                std::atomic_thread_fence(std::memory_order_seq_cst);
                break;
        }
    }

    inline void atomicFence(Concurrency::MemoryOrder order)
    {
        switch (order)
        {
            case Concurrency::MemoryOrder::Relaxed:
                std::atomic_signal_fence(std::memory_order_relaxed);
                break;
            case Concurrency::MemoryOrder::Acquire:
                std::atomic_signal_fence(std::memory_order_acquire);
                break;
            case Concurrency::MemoryOrder::Release:
                std::atomic_signal_fence(std::memory_order_release);
                break;
            case Concurrency::MemoryOrder::AcquireRelease:
                std::atomic_signal_fence(std::memory_order_acq_rel);
                break;
            case Concurrency::MemoryOrder::SequentiallyConsistent:
                std::atomic_signal_fence(std::memory_order_seq_cst);
                break;
        }
    }
}
