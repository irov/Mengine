#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2ThreadMutex
        : public ThreadMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL2ThreadMutex );

    public:
        SDL2ThreadMutex();
        ~SDL2ThreadMutex() override;

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
    typedef IntrusivePtr<SDL2ThreadMutex, ThreadMutexInterface> SDL2ThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
