#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidNativeUnityAdsPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeUnityAds" )

    public:
        AndroidNativeUnityAdsPlugin();
        ~AndroidNativeUnityAdsPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
