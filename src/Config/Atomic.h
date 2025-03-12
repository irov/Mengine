#pragma once

#include "Config/StdInt.h"

#include <atomic>

namespace Mengine
{
    template<class T>
    using Atomic = std::atomic<T>;

    typedef Atomic<bool> AtomicBool;
    typedef Atomic<int32_t> AtomicInt32;
    typedef Atomic<uint32_t> AtomicUInt32;
    typedef Atomic<float> AtomicFloat;

    namespace StdAtomic
    {
        using std::memory_order_acquire;
        using std::memory_order_release;
        using std::memory_order_relaxed;
        using std::memory_order_acq_rel;
    }

    namespace Helper
    {
        template<class T>
        MENGINE_CONSTEXPR T atomicLoad( const Atomic<T> & _value )
        {
            T value = _value.load();

            return value;
        }
    }
}