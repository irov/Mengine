#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ThreadMutexDummy
        : public ThreadMutexInterface
        , public Factorable
    {
    protected:
        void lock() override
        {
            //Empty
        };

        void unlock() override
        {
            //Empty
        };

    protected:
        bool try_lock() override
        {
            //Empty

            return true;
        };
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadMutexDummy> ThreadMutexDummyPtr;
    //////////////////////////////////////////////////////////////////////////
}