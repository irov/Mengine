#   pragma once

#   include "Interface/ProfilerInterface.h"

namespace Menge
{
    class ProfilerService
        : public ProfilerServiceInterface
    {
    public:
        ProfilerService();
        ~ProfilerService();

    protected:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    protected:
        bool initialize() override;

    protected:
        void memoryBegin() override;
        size_t memoryEnd() override;

        size_t getMemoryUsage() const override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        void * m_checkpoint;
        size_t m_beginMemoryUsage;
    };
}