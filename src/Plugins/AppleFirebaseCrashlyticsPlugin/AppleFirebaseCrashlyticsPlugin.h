#pragma once

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
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
