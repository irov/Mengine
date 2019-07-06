#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadMutexDummy
        : public ThreadMutexInterface
        , public Factorable
    {
    protected:
        void lock() override;
        void unlock() override;

    protected:
        bool try_lock() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadMutexDummy> ThreadMutexDummyPtr;
    //////////////////////////////////////////////////////////////////////////
}