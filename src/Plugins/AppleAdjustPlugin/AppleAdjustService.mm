#include "AppleAdjustService.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"


//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleAdjustService, Mengine::AppleAdjustService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAdjustService::AppleAdjustService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdjustService::~AppleAdjustService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdjustService::_initializeService()
    {
        const Char * Adjust_AppToken = CONFIG_VALUE("Adjust", "AppToken", "");
        double Adjust_DelayStart = CONFIG_VALUET("Adjust", "StartDelay", 0.0, double);
        
        LOGGER_INFO("adjust", "App Token '%s'"
            , Adjust_AppToken
        );
        
#ifdef MENGINE_DEBUG
        NSString *environment = ADJEnvironmentSandbox;
#else
        NSString *environment = ADJEnvironmentProduction;
#endif
        ADJConfig *adjustConfig = [ADJConfig configWithAppToken:[NSString stringWithUTF8String:Adjust_AppToken]
                                                    environment:environment];
        
#ifdef MENGINE_DEBUG
        [adjustConfig setLogLevel:ADJLogLevelVerbose];
#endif
        [adjustConfig setDelayStart:Adjust_DelayStart];
        
        [Adjust appDidLaunch:adjustConfig];
        
        LOGGER_INFO("adjust","adjustUserId = %s",[Adjust adid].UTF8String);
        
        [Adjust requestTrackingAuthorizationWithCompletionHandler:^(NSUInteger status) {
            switch (status) {
                case 0:
                    LOGGER_INFO("adjust", "ATTrackingManagerAuthorizationStatusNotDetermined");
                    break;
                case 1:
                    LOGGER_INFO("adjust", "ATTrackingManagerAuthorizationStatusRestricted");
                    break;
                case 2:
                    LOGGER_INFO("adjust", "ATTrackingManagerAuthorizationStatusDenied");
                    break;
                case 3:
                    LOGGER_INFO("adjust", "ATTrackingManagerAuthorizationStatusAuthorized");
                    break;
            }
        }];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAdjustService::_finalizeService()
    {
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAdjustService::eventTraking(const ConstString& token) const
    {
        ADJEvent *event = [ADJEvent eventWithEventToken: [NSString stringWithUTF8String:token.c_str()] ];
        [Adjust trackEvent:event];
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAdjustService::revenueTracking(const ConstString& token,double amount,const ConstString& currency) const
    {
        ADJEvent *event = [ADJEvent eventWithEventToken: [NSString stringWithUTF8String:token.c_str()] ];
        [event setRevenue:amount currency:[NSString stringWithUTF8String:currency.c_str()]];

        [Adjust trackEvent:event];
    }
    //////////////////////////////////////////////////////////////////////////
}
