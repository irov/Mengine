#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleStoreInAppPurchasePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleStoreInAppPurchase" )

    public:
        AppleStoreInAppPurchasePlugin();
        ~AppleStoreInAppPurchasePlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;     
    };
}

