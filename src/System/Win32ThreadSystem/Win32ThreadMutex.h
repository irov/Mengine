#   pragma once

#   include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

#   include "Core/ServantBase.h"

namespace Menge
{
    class Win32ThreadMutex
        : public ServantBase<ThreadMutexInterface>
    {
    public:
        Win32ThreadMutex();
        
    public:
        void initialize( const char * _doc );

    protected:
        void lock() override;
        void unlock() override;

	protected:
		bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        CRITICAL_SECTION m_cs;

		const char * m_doc;
    };
}