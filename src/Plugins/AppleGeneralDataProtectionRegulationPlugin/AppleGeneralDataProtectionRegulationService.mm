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
        
        [[AppleGeneralDataProtectionRegulationApplicationDelegate sharedInstance] setGDPRPass:(BOOL)_passGDPR];
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationService::isGDPRPass() const
    {
        BOOL passGDPR = [[AppleGeneralDataProtectionRegulationApplicationDelegate sharedInstance] isGDPRPass];
        
        return passGDPR;
    }
    //////////////////////////////////////////////////////////////////////////
}
