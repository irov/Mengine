#pragma once

#include "Interface/ThreadSharedMutexInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2ThreadSharedMutex
        : public ThreadSharedMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL2ThreadSharedMutex );

    public:
        SDL2ThreadSharedMutex();
        ~SDL2ThreadSharedMutex() override;

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
        SDL_mutex * m_cs;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL2ThreadSharedMutex, ThreadSharedMutexInterface> SDL2ThreadSharedMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
