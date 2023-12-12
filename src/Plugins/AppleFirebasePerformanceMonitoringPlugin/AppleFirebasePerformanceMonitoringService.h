#pragma once

#include "AppleFirebasePerformanceMonitoringInterface.h"

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
        
    public:
        AppleFirebasePerformanceMonitoringTraceInterfacePtr createTrace( const ConstString & _name ) override;
    };
}
