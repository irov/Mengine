#   pragma once

#   include "Interface/ThreadSystemInterface.h"

#   include "s3eThread.h"

namespace Menge
{
    class MarmaladeThreadMutex
        : public ThreadMutexInterface
    {
    public:
        MarmaladeThreadMutex();

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider );

    protected:
        void lock() override;
        void unlock() override;

	protected:
		bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        s3eThreadLock * m_cs;
    };

	typedef stdex::intrusive_ptr<MarmaladeThreadMutex> MarmaladeThreadMutexPtr;
}