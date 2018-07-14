#pragma once

#include "Interface/ThreadInterface.h"

#include "Core/PluginBase.h"

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
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}
