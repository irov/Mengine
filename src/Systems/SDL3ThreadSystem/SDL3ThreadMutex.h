#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL3ThreadMutex
        : public ThreadMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL3ThreadMutex );

    public:
        SDL3ThreadMutex();
        ~SDL3ThreadMutex() override;

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
        SDL_Mutex * m_cs;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL3ThreadMutex, ThreadMutexInterface> SDL3ThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}



