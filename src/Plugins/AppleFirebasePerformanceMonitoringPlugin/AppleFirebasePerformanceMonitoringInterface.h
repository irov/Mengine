#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleFirebasePerformanceMonitoringTraceInterface
        : public ServantInterface
    {
    public:
        virtual void start() = 0;
        virtual void stop() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleFirebasePerformanceMonitoringTraceInterface> AppleFirebasePerformanceMonitoringTraceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleFirebasePerformanceMonitoringServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebasePerformanceMonitoringService" )
        
    public:
        virtual AppleFirebasePerformanceMonitoringTraceInterfacePtr createTrace( const ConstString & _name ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_PERFORMANCEMONITORING_SERVICE()\
    ((Mengine::AppleFirebasePerformanceMonitoringServiceInterface *)SERVICE_GET(Mengine::AppleFirebasePerformanceMonitoringServiceInterface))
//////////////////////////////////////////////////////////////////////////
