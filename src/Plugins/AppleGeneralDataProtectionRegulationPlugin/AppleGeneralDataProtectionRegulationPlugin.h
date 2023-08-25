#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleGeneralDataProtectionRegulationPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleGeneralDataProtectionRegulation" )

    public:
        AppleGeneralDataProtectionRegulationPlugin();
        ~AppleGeneralDataProtectionRegulationPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;     
    };
}

