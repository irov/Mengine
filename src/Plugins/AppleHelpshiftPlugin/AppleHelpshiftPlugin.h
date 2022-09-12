#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleHelpshiftPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleHelpshift" )

    public:
        AppleHelpshiftPlugin();
        ~AppleHelpshiftPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;     
    };
}

