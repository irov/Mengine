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
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
