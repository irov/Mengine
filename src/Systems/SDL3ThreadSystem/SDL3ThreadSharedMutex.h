#pragma once

#include "Interface/ThreadSharedMutexInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL3ThreadSharedMutex
        : public ThreadSharedMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL3ThreadSharedMutex );

    public:
        SDL3ThreadSharedMutex();
        ~SDL3ThreadSharedMutex() override;

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
        SDL_Mutex * m_cs;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL3ThreadSharedMutex, ThreadSharedMutexInterface> SDL3ThreadSharedMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}



