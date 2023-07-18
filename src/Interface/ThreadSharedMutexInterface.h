#pragma once

#include "Interface/ThreadMutexInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadSharedMutexInterface
        : public ThreadMutexInterface
    {
    public:
        virtual void lockShared() = 0;
        virtual void unlockShared() = 0;
        virtual bool tryLockShared() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadSharedMutexInterface, ThreadMutexInterface> ThreadSharedMutexInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}