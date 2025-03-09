#pragma once

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
        bool _initializePlugin() override;
        void _finalizePlugin() override; 
    };
}
