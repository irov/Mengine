#include "AppleGeneralDataProtectionRegulationService.h"

#import "AppleGeneralDataProtectionRegulationApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/Apple/AppleUserDefaults.h"

#include "Kernel/Logger.h"

#import <UIKit/UIKit.h>

////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleGeneralDataProtectionRegulationService, Mengine::AppleGeneralDataProtectionRegulationService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationService::AppleGeneralDataProtectionRegulationService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationService::~AppleGeneralDataProtectionRegulationService()
    {
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationService::_initializeService()
    {
        //Empty
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleGeneralDataProtectionRegulationService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGeneralDataProtectionRegulationService::setGDPRPass( bool _passGDPR )
    {
        LOGGER_MESSAGE( "set GDPR pass [%d]"
            , _passGDPR
        );
        
        AppleGeneralDataProtectionRegulationApplicationDelegate * delegate = [iOSDetail getUIProxyApplicationDelegate:[AppleGeneralDataProtectionRegulationApplicationDelegate class]];
        
        [delegate setGDPRPass:(BOOL)_passGDPR];
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationService::isGDPRPass() const
    {
        AppleGeneralDataProtectionRegulationApplicationDelegate * delegate = [iOSDetail getUIProxyApplicationDelegate:[AppleGeneralDataProtectionRegulationApplicationDelegate class]];
        
        BOOL passGDPR = [delegate isGDPRPass];
        
        return passGDPR;
    }
    //////////////////////////////////////////////////////////////////////////
}
