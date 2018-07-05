#pragma once

#include "Core/PluginBase.h"

#include "UnitySetupCallback.h"
#include "UnityShowAdCallback.h"

namespace Mengine 
{
    class AndroidNativeUnityPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeUnity" )

    public:
        AndroidNativeUnityPlugin();
        ~AndroidNativeUnityPlugin() override;
    public:
        bool setupAds( const bool debug, const UnitySetupCallbackPtr & _callback );
        bool showAd( const UnityShowAdCallbackPtr & _callback );
    };
}
