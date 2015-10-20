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
		bool initialize( ServiceProviderInterface * _serviceProvider, const char * _doc );

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

		const char * m_doc;
    };

	typedef stdex::intrusive_ptr<MarmaladeThreadMutex> MarmaladeThreadMutexPtr;
}