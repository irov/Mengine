#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleAdjustPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleAdjust" )

    public:
        AppleAdjustPlugin();
        ~AppleAdjustPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;        
    };
}
