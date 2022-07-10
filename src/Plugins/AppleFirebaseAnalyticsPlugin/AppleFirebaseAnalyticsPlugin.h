#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleFirebaseAnalyticsPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleFirebaseAnalytics" )

    public:
        AppleFirebaseAnalyticsPlugin();
        ~AppleFirebaseAnalyticsPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
