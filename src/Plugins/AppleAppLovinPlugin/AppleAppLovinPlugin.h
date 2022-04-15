#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleAppLovinPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleAppLovin" )

    public:
        AppleAppLovinPlugin();
        ~AppleAppLovinPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;        
    };
}
