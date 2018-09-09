#pragma once

#include <atomic>
#include <stdint.h>

namespace Mengine
{
    template<class T>
    using Atomic = std::atomic<T>;

    typedef Atomic<bool> AtomicBool;
    typedef Atomic<uint32_t> AtomicUInt32;
}