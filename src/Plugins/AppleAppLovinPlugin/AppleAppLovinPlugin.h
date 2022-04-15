#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppLovinPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Appodeal" )

    public:
        AppLovinPlugin();
        ~AppLovinPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;        
    };
}
