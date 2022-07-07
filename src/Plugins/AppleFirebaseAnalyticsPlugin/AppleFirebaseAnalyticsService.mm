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
    bool AppleFirebaseAnalyticsService::sendEvent(const ConstString& _name,const FirebaseAnalyticsParams& _params)
    {
        LOGGER_INFO( "FirebaseAnalytics", "sendEvent" );
        
        NSMutableDictionary *event_params = [[NSMutableDictionary alloc] init];
        for(FirebaseAnalyticsParams::const_iterator it = _params.begin(),it_e = _params.end(); it != it_e; ++it)
        {
            [event_params setObject:[NSString stringWithUTF8String:it->second.c_str()]  forKey:[NSString stringWithUTF8String:it->first.c_str()]];
        }
        
        [FIRAnalytics logEventWithName:[NSString stringWithUTF8String: _name.c_str()]
                            parameters:event_params];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
