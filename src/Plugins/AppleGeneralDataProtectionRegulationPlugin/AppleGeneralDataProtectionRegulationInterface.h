#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleGeneralDataProtectionRegulationServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleGeneralDataProtectionRegulationServiceInterface" )
            
    public:
        virtual void setGDPRPass( bool _passGDPR ) = 0;
        virtual bool isGDPRPass() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_GENERALDATAPROTECTIONREGULATION_SERVICE()\
    ((Mengine::AppleGeneralDataProtectionRegulationServiceInterface *)SERVICE_GET(Mengine::AppleGeneralDataProtectionRegulationServiceInterface))
//////////////////////////////////////////////////////////////////////////
