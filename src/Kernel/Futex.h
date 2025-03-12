#pragma once

#include "Config/Atomic.h"
#include "Config/StdThread.h"

namespace Mengine
{
    class Futex
    {
    public:
        Futex();
        ~Futex();

    public:
        void lock();
        void unlock();

    protected:
        Atomic<StdThread::thread::id> m_owner;
        AtomicInt32 m_lock;
    };
}