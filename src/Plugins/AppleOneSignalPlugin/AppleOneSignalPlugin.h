#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleOneSignalPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleOneSignal" )

    public:
        AppleOneSignalPlugin();
        ~AppleOneSignalPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;        
    };
}
