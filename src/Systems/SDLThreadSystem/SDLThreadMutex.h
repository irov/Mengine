#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Factorable.h"

#include "Config/String.h"

#include "SDL_thread.h"

namespace Mengine
{
    class SDLThreadMutex
        : public ThreadMutexInterface
		, public Factorable
    {
    public:
        SDLThreadMutex();
		~SDLThreadMutex() override;

    public:
        bool initialize( const Char * _doc );

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
		String m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadMutex> SDLThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
