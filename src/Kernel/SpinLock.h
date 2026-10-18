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
        bool tryLock();
        void unlock();

    protected:
        Atomic<StdThread::thread::id> m_owner;
        int32_t m_lock;
    };
}