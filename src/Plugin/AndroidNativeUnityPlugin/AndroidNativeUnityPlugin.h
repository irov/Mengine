#pragma once

#include "Core/PluginBase.h"

#include "UnityAdEventHandler.h"

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
        bool setupAds( bool _debug );
        bool showAd( const String & _placementId );
        bool setAdsEventHandler( const UnityAdEventHandlerPtr & _callback );
    };
}
