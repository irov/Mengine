#pragma once

#include "Interface/ThreadSharedMutexInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadSharedMutexDummy
        : public ThreadSharedMutexInterface
        , public Factorable
    {
    public:
        ThreadSharedMutexDummy();
        ~ThreadSharedMutexDummy() override;

    protected:
        void lock() override;
        void unlock() override;
        bool tryLock() override;
        void lockShared() override;
        void unlockShared() override;
        bool tryLockShared() override;
    };
    //////////////////////////////////////////////////////////////////////////
}
