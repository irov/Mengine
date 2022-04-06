#pragma once

#include "Interface/PlatformInterface.h"

#import "AppleMARSDKNative.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleMARSDKPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleMARSDK" )

    public:
        AppleMARSDKPlugin();
        ~AppleMARSDKPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;        
    };
}
