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

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool setupAds( bool _debug, const UnitySetupCallbackPtr & _callback );
        bool showAd( const String & _placementId, const UnityShowAdCallbackPtr & _callback );
    };
}
