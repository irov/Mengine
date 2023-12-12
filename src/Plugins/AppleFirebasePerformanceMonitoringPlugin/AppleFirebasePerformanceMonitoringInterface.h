#pragma once

#include "Interface/ServiceInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleFirebasePerformanceMonitoringServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebasePerformanceMonitoringService" )
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_PERFORMANCEMONITORING_SERVICE()\
    ((Mengine::AppleFirebasePerformanceMonitoringServiceInterface *)SERVICE_GET(Mengine::AppleFirebasePerformanceMonitoringServiceInterface))
//////////////////////////////////////////////////////////////////////////
