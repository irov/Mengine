#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Factorable.h"

#include <pthread.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class POSIXThreadMutex
        : public ThreadMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( POSIXThreadMutex );

    public:
        POSIXThreadMutex();
        ~POSIXThreadMutex() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void lock() override;
        void unlock() override;
        bool tryLock() override;

    protected:
        void _destroy() override;

    protected:
        pthread_mutex_t m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<POSIXThreadMutex, ThreadMutexInterface> POSIXThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
