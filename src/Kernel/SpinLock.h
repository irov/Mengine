#pragma once

#include "Config/Atomic.h"
#include "Config/StdThread.h"

namespace Mengine
{
    class SpinLock
    {
    public:
        SpinLock();
        ~SpinLock();

    public:
        void lock();
        void unlock();

    protected:
        MENGINE_ALIGNAS( 64 ) Atomic<size_t> m_owner;
        MENGINE_ALIGNAS( 64 ) AtomicInt32 m_lock;
    };
}