#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

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
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        AnalyticsEventProviderInterfacePtr m_provider;
    };
}
