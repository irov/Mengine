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
        AtomicUInt32 m_owner;
        AtomicInt32 m_lock;
    };
}