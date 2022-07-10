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
        @try {
            [FIRApp configure];
        }
        @catch (NSException *exception) {
            LOGGER_ERROR( "%s"
                , [exception.reason UTF8String]
            );
        }
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsService::sendEvent( const ConstString & _name, const FirebaseAnalyticsParams & _params )
    {
        LOGGER_INFO( "firebaseanalytics", "sendEvent" );
        
        NSMutableDictionary * event_params = [[NSMutableDictionary alloc] init];

        for( auto && [key, value] : _params )
        {
            LOGGER_INFO( "firebaseanalytics", "['%s' : '%s']"
                , key.c_str()
                , value.c_str()
            );

            const Char * key_str = key.c_str();
            const Char * value_str = value.c_str();

            [event_params setObject:[NSString stringWithUTF8String:value_str]  forKey:[NSString stringWithUTF8String:key_str]];
        }

        const Char * name_str = _name.c_str();
        
        [FIRAnalytics logEventWithName:[NSString stringWithUTF8String: name_str]
            parameters:event_params
        ];
    }
    //////////////////////////////////////////////////////////////////////////
}
