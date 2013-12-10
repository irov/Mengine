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
        void initialize( ServiceProviderInterface * _serviceProvider );

    protected:
        void lock() override;
        void unlock() override;

    protected:
        void _destroy() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        s3eThreadLock * m_cs;
    };
}