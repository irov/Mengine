#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

#include "SDL_thread.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLThreadMutex
        : public ThreadMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLFileOutputStream );

    public:
        SDLThreadMutex();
        ~SDLThreadMutex() override;

    public:
        bool initialize( const DocumentPtr & _doc );
        void finalize();

    protected:
        void lock() override;
        void unlock() override;

    protected:
        bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        SDL_mutex * m_cs;

#ifdef MENGINE_DEBUG
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadMutex, ThreadMutexInterface> SDLThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
