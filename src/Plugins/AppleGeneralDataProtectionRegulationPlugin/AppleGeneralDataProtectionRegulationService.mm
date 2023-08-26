#include "AppleGeneralDataProtectionRegulationService.h"

#include "Environment/Apple/AppleUserDefaults.h"

#include "Kernel/Logger.h"

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
        bool passGDPR = Helper::AppleGetUserDefaultsBoolean( "mengine.gdpr.pass", false );
        
        m_passGDPR = passGDPR;
        
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
        
        Helper::AppleSetUserDefaultsBoolean( "mengine.gdpr.pass", _passGDPR );

        m_passGDPR = _passGDPR;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationService::isGDPRPass() const
    {
        return m_passGDPR;
    }
    //////////////////////////////////////////////////////////////////////////
}
