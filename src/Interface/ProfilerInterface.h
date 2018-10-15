#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class ProfilerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ProfilerService" )

    public:
        virtual void memoryBegin() = 0;
        virtual size_t memoryEnd() = 0;

        virtual size_t getMemoryUsage() const = 0;
    };

#define PROFILER_SERVICE()\
    SERVICE_GET(Mengine::ProfilerServiceInterface)
}