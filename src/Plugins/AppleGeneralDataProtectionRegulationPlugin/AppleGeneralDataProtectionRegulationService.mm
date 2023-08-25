#include "AppleGeneralDataProtectionRegulationService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleGeneralDataProtectionRegulationService, Mengine::AppleGeneralDataProtectionRegulationService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationService::AppleGeneralDataProtectionRegulationService()
        : m_passGDPR(false)
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

        m_passGDPR = _passGDPR;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationService::isGDPRPass() const
    {
        return m_passGDPR;
    }
    //////////////////////////////////////////////////////////////////////////
}
