#pragma once

#include "Interface/ThreadSharedMutexInterface.h"

#include "Environment/SDL/SDLIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLThreadSharedMutex
        : public ThreadSharedMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLThreadSharedMutex );

    public:
        SDLThreadSharedMutex();
        ~SDLThreadSharedMutex() override;

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
    typedef IntrusivePtr<SDLThreadSharedMutex, ThreadSharedMutexInterface> SDLThreadSharedMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
