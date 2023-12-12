#pragma once

#include "AppleFirebasePerformanceMonitoringServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleFirebasePerformanceMonitoringService
        : public ServiceBase<AppleFirebasePerformanceMonitoringServiceInterface>
    {
    public:
        AppleFirebasePerformanceMonitoringService();
        ~AppleFirebasePerformanceMonitoringService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
