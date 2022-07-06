#include "AppleFirebaseAnalyticsService.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

#import <Firebase/Firebase.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleFirebaseAnalyticsService, Mengine::AppleFirebaseAnalyticsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseAnalyticsService::AppleFirebaseAnalyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseAnalyticsService::~AppleFirebaseAnalyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseAnalyticsService::_initializeService()
    {
        [FIRApp configure];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////
    bool AppleFirebaseAnalyticsService::sendEvent()
    {
        LOGGER_INFO( "FirebaseAnalytics", "sendEvent" );
        
        [FIRAnalytics logEventWithName:kFIREventSelectContent
                            parameters:@{
                                         kFIRParameterItemID:[NSString stringWithFormat:@"id-%@", @"self.title"],
                                         kFIRParameterItemName:@"title",
                                         kFIRParameterContentType:@"image"
                                         }];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
