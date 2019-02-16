#pragma once

#include "Kernel/Mixin.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadMutexInterface
        : public Mixin
    {
    public:
        virtual void lock() = 0;
        virtual void unlock() = 0;

    public:
        virtual bool try_lock() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadMutexInterface> ThreadMutexInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}