#   pragma once

#   include "Interface/ThreadSystemInterface.h"

#   include "Core/ServantBase.h"

#	include "SDL_Thread.h"

namespace Menge
{
    class SDLThreadMutex
        : public ServantBase<ThreadMutexInterface>
    {
    public:
        SDLThreadMutex();

    public:
        bool initialize( const char * _file, uint32_t _line );

    protected:
        void lock() override;
        void unlock() override;

    protected:
        bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        SDL_mutex * m_cs;

		const char * m_file;
		uint32_t m_line;
    };

    typedef stdex::intrusive_ptr<SDLThreadMutex> SDLThreadMutexPtr;
}