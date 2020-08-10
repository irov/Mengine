#pragma once

#include "Config/StdInt.h"

#include <atomic>

namespace Mengine
{
    template<class T>
    using Atomic = std::atomic<T>;

    typedef Atomic<bool> AtomicBool;
    typedef Atomic<uint32_t> AtomicUInt32;
    typedef Atomic<float> AtomicFloat;
}