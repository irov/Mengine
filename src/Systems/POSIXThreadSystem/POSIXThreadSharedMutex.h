#pragma once

#include "Interface/ThreadSharedMutexInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

#include <pthread.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class POSIXThreadSharedMutex
        : public ThreadSharedMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( POSIXThreadSharedMutex );

    public:
        POSIXThreadSharedMutex();
        ~POSIXThreadSharedMutex() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void lock() override;
        void unlock() override;
        bool tryLock() override;

    public:
        void lockShared() override;
        void unlockShared() override;
        bool tryLockShared() override;

    protected:
        void _destroy() override;

    protected:
        pthread_rwlock_t m_rwlock;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<POSIXThreadSharedMutex, ThreadSharedMutexInterface> POSIXThreadSharedMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
