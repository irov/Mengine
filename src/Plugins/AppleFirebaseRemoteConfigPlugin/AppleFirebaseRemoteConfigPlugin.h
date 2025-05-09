#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleFirebaseRemoteConfigPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleFirebaseRemoteConfig" )

    public:
        AppleFirebaseRemoteConfigPlugin();
        ~AppleFirebaseRemoteConfigPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
