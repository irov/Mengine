#   pragma once

#include "Interface/ThreadSystemInterface.h"

#include "WIN32/WindowsIncluder.h"

#include "Core/ServantBase.h"

namespace Mengine
{
    class Win32ThreadMutex
        : public ServantBase<ThreadMutexInterface>
    {
    public:
        Win32ThreadMutex();
        
    public:
        void initialize( const char * _file, uint32_t _line );

    protected:
        void lock() override;
        void unlock() override;

	protected:
		bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        CRITICAL_SECTION m_cs;

		const char * m_file;
		uint32_t m_line;
    };
}