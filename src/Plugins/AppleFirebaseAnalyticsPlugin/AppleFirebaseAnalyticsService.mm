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
        } @catch (NSException *exception) {
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
    ////////////////////////////////////////////////////////////////////////
    NSString * AppleFirebaseAnalyticsService::getkFIREventAdImpression() const
    {
        return kFIREventAdImpression;
    }
    //////////////////////////////////////////////////////////////////////
    NSString * AppleFirebaseAnalyticsService::getkFIRParameterAdPlatform() const
    {
        return kFIRParameterAdPlatform;
    }
    //////////////////////////////////////////////////////////////////////
    NSString * AppleFirebaseAnalyticsService::getkFIRParameterAdSource() const
    {
        return kFIRParameterAdSource;
    }
    //////////////////////////////////////////////////////////////////////
    NSString * AppleFirebaseAnalyticsService::getkFIRParameterAdFormat() const
    {
        return kFIRParameterAdFormat;
    }
    //////////////////////////////////////////////////////////////////////
    NSString * AppleFirebaseAnalyticsService::getkFIRParameterAdUnitName() const
    {
        return kFIRParameterAdUnitName;
    }
    //////////////////////////////////////////////////////////////////////
    NSString * AppleFirebaseAnalyticsService::getkFIRParameterCurrency() const
    {
        return kFIRParameterCurrency;
    }
    //////////////////////////////////////////////////////////////////////
    NSString * AppleFirebaseAnalyticsService::getkFIRParameterValue() const
    {
        return kFIRParameterValue;
    }
    //////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsService::sendEvent( NSString * _name, NSDictionary<NSString *, id> * _parameters )
    {
        LOGGER_INFO( "firebaseanalytics", "sendEvent %s"
            , [ _name UTF8String]
        );
        
        for (NSString * key in _parameters)
        {
            id value = _parameters[key];
            
            LOGGER_INFO( "firebaseanalytics", "['%s' : '%s']"
                , [key UTF8String]
                , [[value description] UTF8String]
            );
        }

        [FIRAnalytics logEventWithName:_name parameters:_parameters];
    }
    //////////////////////////////////////////////////////////////////////////
}
