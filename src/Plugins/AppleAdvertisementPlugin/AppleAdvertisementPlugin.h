#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleAdvertisementPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleAdvertisement" )

    public:
        AppleAdvertisementPlugin();
        ~AppleAdvertisementPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;        
    };
}
