#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleAmplitudePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleAmplitude" )

    public:
        AppleAmplitudePlugin();
        ~AppleAmplitudePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        AnalyticsEventProviderInterfacePtr m_provider;
    };
}

