#   pragma once

#   include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
    class Win32ThreadMutex
        : public ThreadMutexInterface
    {
    public:
        Win32ThreadMutex();
        
    public:
        void initialize( ServiceProviderInterface * _serviceProvider );

    protected:
        void lock() override;
        void unlock() override;

	protected:
		bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        CRITICAL_SECTION m_cs;
    };
}