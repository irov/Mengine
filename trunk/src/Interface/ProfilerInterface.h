#   pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
    class ProfilerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ProfilerService")

    public:
        virtual bool initialize() = 0;

        virtual void memoryBegin() = 0;
        virtual size_t memoryEnd() = 0;

        virtual size_t getMemoryUsage() const = 0;
    };

#   define PROFILER_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::ProfilerServiceInterface)
}