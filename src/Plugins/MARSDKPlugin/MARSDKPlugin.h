#pragma once

#include "Interface/PlatformInterface.h"
#include "Interface/SDLPlatformExtensionInterface.h"

#import "IOSMarSDKNative.h"

#include "Kernel/PluginBase.h"


namespace Mengine
{    
    class MARSDKPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "MARSDK" )

    public:
        MARSDKPlugin();
        ~MARSDKPlugin() override;

    protected:
        bool _unimportantPlugin() const override;
        
    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
        
    protected:
        IOSMarSDKNative * m_marSDKNative;
    };
}
