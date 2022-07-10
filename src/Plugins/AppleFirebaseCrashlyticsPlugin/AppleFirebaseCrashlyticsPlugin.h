#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleFirebaseCrashlyticsPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleFirebaseCrashlytics" )

    public:
        AppleFirebaseCrashlyticsPlugin();
        ~AppleFirebaseCrashlyticsPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
