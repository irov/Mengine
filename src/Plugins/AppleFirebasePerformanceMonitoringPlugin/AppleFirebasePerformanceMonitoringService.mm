#include "AppleFirebasePerformanceMonitoringService.h"

#import "Environment/Apple/AppleString.h"

#import <FirebasePerformance/FirebasePerformance.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleFirebasePerformanceMonitoringService, Mengine::AppleFirebasePerformanceMonitoringService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebasePerformanceMonitoringService::AppleFirebasePerformanceMonitoringService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebasePerformanceMonitoringService::~AppleFirebasePerformanceMonitoringService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebasePerformanceMonitoringService::_initializeService()
    {
        //Empty
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFirebasePerformanceMonitoringService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebasePerformanceMonitoringTraceInterfacePtr AppleFirebasePerformanceMonitoringService::createTrace( const ConstString & _name )
    {
        //FIRTrace *trace = [[FIRPerformance sharedInstance] traceWithName:name];
        
        return AppleFirebasePerformanceMonitoringTraceInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
}
