#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Factorable.h"

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
        bool initialize( const Char * _doc, uint32_t _line );

    protected:
        void lock() override;
        void unlock() override;

    protected:
        bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        SDL_mutex * m_cs;

		const Char * m_file;
		uint32_t m_line;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadMutex> SDLThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
