#pragma once

#include "Kernel/PluginBase.h"
#include "IOSMarSDKNative.h"

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
        
        IOSMarSDKNative * m_marSDKNative;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
