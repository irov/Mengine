#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine 
{
    class AndroidNativeAdMobPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeAdMob" )

    public:
        AndroidNativeAdMobPlugin();
        ~AndroidNativeAdMobPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}
