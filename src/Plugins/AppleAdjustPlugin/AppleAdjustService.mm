#include "AppleAdjustService.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"

#import <Adjust/Adjust.h>

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
        LOGGER_MESSAGE( "adid: %s"
            , [[Adjust adid] UTF8String]
        );
        
        [Adjust requestTrackingAuthorizationWithCompletionHandler:^(NSUInteger status) {
            switch (status) {
                case 0:
                    LOGGER_MESSAGE( "ATTrackingManagerAuthorizationStatusNotDetermined" );
                    break;
                case 1:
                    LOGGER_MESSAGE( "ATTrackingManagerAuthorizationStatusRestricted" );
                    break;
                case 2:
                    LOGGER_MESSAGE( "ATTrackingManagerAuthorizationStatusDenied" );
                    break;
                case 3:
                    LOGGER_MESSAGE( "ATTrackingManagerAuthorizationStatusAuthorized" );
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
    void AppleAdjustService::eventTraking( const ConstString & _token )
    {
        LOGGER_MESSAGE( "eventTraking token: %s"
            , _token.c_str()
        );
        
        ADJEvent *event = [ADJEvent eventWithEventToken: [NSString stringWithUTF8String:_token.c_str()]];
        
        [Adjust trackEvent:event];
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAdjustService::revenueTracking( const ConstString & _token, double _amount, const ConstString & _currency )
    {
        LOGGER_MESSAGE( "revenueTracking token: %s anount: %lf currency: %s"
            , _token.c_str()
            , _amount
            , _currency.c_str()
        );
        
        ADJEvent *event = [ADJEvent eventWithEventToken: [NSString stringWithUTF8String:_token.c_str()]];
        
        [event setRevenue:_amount currency:[NSString stringWithUTF8String:_currency.c_str()]];

        [Adjust trackEvent:event];
    }
    //////////////////////////////////////////////////////////////////////////
}
