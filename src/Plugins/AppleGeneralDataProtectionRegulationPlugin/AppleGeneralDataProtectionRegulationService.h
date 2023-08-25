#pragma once

#include "AppleGeneralDataProtectionRegulationInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleGeneralDataProtectionRegulationService
         : public ServiceBase<AppleGeneralDataProtectionRegulationServiceInterface>
    {
    public:
        AppleGeneralDataProtectionRegulationService();
        ~AppleGeneralDataProtectionRegulationService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        void setGDPRPass( bool _passGDPR ) override;
        bool isGDPRPass() const override;
    };
}
