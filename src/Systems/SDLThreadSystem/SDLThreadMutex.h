#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Environment/SDL/SDLIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLThreadMutex
        : public ThreadMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLThreadMutex );

    public:
        SDLThreadMutex();
        ~SDLThreadMutex() override;

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
        SDL_mutex * m_cs;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadMutex, ThreadMutexInterface> SDLThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
